// From http://stackoverflow.com/a/12413298
// New Implementation from https://stackoverflow.com/questions/43588275/exceptions-in-stl-streams?noredirect=1&lq=1
// ----------------------------------------------------------------------------
//  Copyright (C) 2013 Dietmar Kuehl http://www.dietmar-kuehl.de
//
//  Permission is hereby granted, free of charge, to any person
//  obtaining a copy of this software and associated documentation
//  files (the "Software"), to deal in the Software without restriction,
//  including without limitation the rights to use, copy, modify,
//  merge, publish, distribute, sublicense, and/or sell copies of
//  the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be
//  included in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <streambuf>
#include <string>
#include <vector>
#include <iostream>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

// ----------------------------------------------------------------------------
// From https://stackoverflow.com/questions/43588275/exceptions-in-stl-streams?noredirect=1&lq=1
// basic_pipebuf is a different implementation of threadbuf
// basic_pipebuf resolves a bug (deadlock) found in threadbuf on debian10 platform

template <class _Elem,
          size_t BUFFER_SIZE = 64 * 1024,
          size_t CHUNK_SIZE = 16 * 1024,
          class _Traits = std::char_traits<_Elem> >
class streambuf_mtsafe : public std::basic_streambuf<_Elem, _Traits> {
public:
    using traits_type = std::streambuf::traits_type;
    using buffer_type = typename std::array<_Elem, BUFFER_SIZE>;
    using buffer_size_type = typename buffer_type::size_type;

    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_closed;

    buffer_type m_buffer;
    _Elem* m_begin;
    _Elem* m_end;
    const buffer_size_type m_chunk_size{CHUNK_SIZE};

public:
    streambuf_mtsafe()
        : m_closed(false), m_buffer(), m_begin(m_buffer.data()), m_end(m_buffer.data() + m_buffer.size()) {
        this->setg(m_begin, m_begin + m_chunk_size, m_begin + m_chunk_size);
        this->setp(m_begin + m_chunk_size, m_begin + 2 * m_chunk_size);
    }

    void close() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_closed = true;
        m_condition.notify_all();
    }

private:
    traits_type::int_type overflow(traits_type::int_type c) override {
        traits_type::int_type ret = traits_type::eof();

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_closed) {
                return traits_type::eof();
            }

            if (traits_type::eq_int_type(traits_type::eof(), c))
                return traits_type::not_eof(c);

            if (this->epptr() < m_end) {
                while (this->epptr() == this->eback() && !m_closed) {
                    m_condition.wait(lock);
                }

                if (this->epptr() != this->eback()) {
                    this->setp(this->pbase() + m_chunk_size, this->epptr() + m_chunk_size);
                    ret = this->sputc(c);
                }
            } else {
                while (!(this->eback() > m_begin || m_closed))
                    m_condition.wait(lock);

                if (this->eback() > m_begin) {
                    this->setp(m_begin, m_begin + m_chunk_size);
                    ret = this->sputc(c);
                }
            }
        }
        m_condition.notify_one();
        return ret;
    }

    traits_type::int_type underflow() override {
        std::unique_lock<std::mutex> lock(m_mutex);
        traits_type::int_type ret = traits_type::eof();

        if (this->eback() != this->pbase()) {
            if (this->egptr() < m_end) {
                while (this->egptr() == this->pbase() && !m_closed) {
                    m_condition.wait(lock);
                }

                if (this->egptr() != this->pbase()) {
                    this->setg(this->eback() + m_chunk_size, this->eback() + m_chunk_size,
                               this->egptr() + m_chunk_size);
                } else {
                    this->setg(this->pbase(), this->pbase(), this->pptr());
                }
            } else {
                while (!(this->pbase() > m_begin || m_closed))
                    m_condition.wait(lock);

                if (this->pbase() > m_begin) {
                    this->setg(m_begin, m_begin, m_begin + m_chunk_size);
                } else if (m_closed) {
                    this->setg(this->pbase(), this->pbase(), this->pptr());
                }
            }
            ret = traits_type::to_int_type(*this->gptr());
        }

        m_condition.notify_one();
        return ret;
    }
};

using streambuf_mtsafe_char = streambuf_mtsafe<char>;

struct Pipe {
    streambuf_mtsafe_char sbuf;
    std::ostream out;
    std::istream in;

    void close() { sbuf.close(); }
    Pipe() : out(&sbuf), in(&sbuf) {}
    Pipe(const Pipe&) = delete;
    Pipe& operator=(const Pipe&) = delete;
    Pipe(const Pipe&&) = delete;
    Pipe& operator=(const Pipe&&) = delete;
    ~Pipe() { close(); }
};

/*
 * CloneHelper is a utility class that allow clone of data not easily copyable
 * It uses serialization to create an archive then deserialization to create a clone
 * To avoid holding 3 copy at the same time, we use a pipe buffer.
 * This pipe is Thread safe, then we can serialize data in one thread, and
 * deserialize data in another one.
 * NOTE : We use boost::archive::no_codecvt, to avoid data race in the  Pipe
 * internal buffer (streambuf) -> std::local is shared for read/write in streambuf
 * */
struct CloneHelper {
    Pipe pipe;

    template <class T>
    void operator()(const T& from, T& to) {
        std::thread write([&]() {
            boost::archive::binary_oarchive oa(pipe.out, boost::archive::no_codecvt);
            oa << from;
            pipe.close();
        });

        {
            boost::archive::binary_iarchive ia(pipe.in, boost::archive::no_codecvt);
            ia >> to;
        }
        if (write.joinable())
            write.join();
    }
};
