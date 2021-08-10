// From http://stackoverflow.com/a/12413298
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

#include <condition_variable>
#include <mutex>
#include <streambuf>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------

class threadbuf : public std::streambuf {
private:
    using traits_type = std::streambuf::traits_type;
    using string_size_t = std::string::size_type;

    std::mutex d_mutex;
    std::condition_variable d_condition;
    std::string d_out;
    std::string d_in;
    std::string d_tmp;
    char* d_current;
    bool d_closed;

public:
    threadbuf(string_size_t size = 64 * 1024)
        : d_out(std::max(string_size_t(1), size), ' '),
          d_in(std::max(string_size_t(1), size), ' '),
          d_tmp(std::max(string_size_t(1), size), ' '),
          d_current(&this->d_tmp[0]),
          d_closed(false) {
        this->setp(&this->d_out[0], &this->d_out[0] + this->d_out.size() - 1);
        this->setg(&this->d_in[0], &this->d_in[0], &this->d_in[0]);
    }
    ~threadbuf() override;
    void close() {
        {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            this->d_closed = true;
            while (this->pbase() != this->pptr()) {
                this->internal_sync(lock);
            }
        }
        this->d_condition.notify_all();
    }

private:
    int_type underflow() override {
        if (this->gptr() == this->egptr()) {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            while (&this->d_tmp[0] == this->d_current && !this->d_closed) {
                this->d_condition.wait(lock);
            }
            if (&this->d_tmp[0] != this->d_current) {
                std::streamsize size(this->d_current - &this->d_tmp[0]);
                traits_type::copy(this->eback(), &this->d_tmp[0], this->d_current - &this->d_tmp[0]);
                this->setg(this->eback(), this->eback(), this->eback() + size);
                this->d_current = &this->d_tmp[0];
                this->d_condition.notify_one();
            }
        }
        return this->gptr() == this->egptr() ? traits_type::eof() : traits_type::to_int_type(*this->gptr());
    }
    int_type overflow(int_type c) override {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        if (!traits_type::eq_int_type(c, traits_type::eof())) {
            *this->pptr() = traits_type::to_char_type(c);
            this->pbump(1);
        }
        return this->internal_sync(lock) ? traits_type::eof() : traits_type::not_eof(c);
    }
    int sync() override {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        return this->internal_sync(lock);
    }
    int internal_sync(std::unique_lock<std::mutex>& lock) {
        char* end(&this->d_tmp[0] + this->d_tmp.size());
        while (this->d_current == end && !this->d_closed) {
            this->d_condition.wait(lock);
        }
        if (this->d_current != end) {
            std::streamsize size(std::min(end - d_current, this->pptr() - this->pbase()));
            traits_type::copy(d_current, this->pbase(), size);
            this->d_current += size;
            std::streamsize remain((this->pptr() - this->pbase()) - size);
            traits_type::move(this->pbase(), this->pptr(), remain);
            this->setp(this->pbase(), this->epptr());
            this->pbump(remain);
            this->d_condition.notify_one();
            return 0;
        }
        return traits_type::eof();
    }
};

template<class _Elem, class _Traits = std::char_traits<_Elem>>
class basic_pipebuf : public std::basic_streambuf<_Elem, _Traits>
{
public:
    using traits_type = std::streambuf::traits_type;
    using buffer_type = typename std::vector<_Elem>;
    using buffer_size_type = typename buffer_type::size_type;

    std::mutex                      m_mutex;
    std::condition_variable         m_condition;
    bool                            m_closed;

    buffer_type                     m_buffer;
    _Elem*                          m_begin;
    _Elem*                          m_end;
    buffer_size_type                m_chunk_size;

public:
    basic_pipebuf(buffer_size_type buffer_size = 16 * 1024, buffer_size_type chunk_size = 16):
        m_closed(false),
        m_buffer(buffer_size),
        m_begin(&m_buffer[0]),
        m_end(&m_buffer[0] + m_buffer.size()),
        m_chunk_size(chunk_size)
    {
        this->setp(m_begin + m_chunk_size, m_begin + 2 * m_chunk_size);
        this->setg(m_begin, m_begin + m_chunk_size, m_begin + m_chunk_size);
    }

    void close(){
        std::unique_lock<std::mutex> lock(m_mutex);
        m_closed = true;
        m_condition.notify_all();
    }

private:
    traits_type::int_type overflow(traits_type::int_type c) override
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_closed)
        {
            return traits_type::eof();
        }

        if (traits_type::eq_int_type(traits_type::eof(), c))
            return traits_type::not_eof(c);

        traits_type::int_type ret = traits_type::eof();

        if (this->epptr() < m_end)
        {
            while (this->epptr() == this->eback() && !m_closed)
                m_condition.wait(lock);

            if (this->epptr() != this->eback())
            {
                this->setp(this->pbase() + m_chunk_size, this->epptr() + m_chunk_size);
                ret = this->sputc(c);
            }
        }
        else
        {
            while (!(this->eback() > m_begin || m_closed))
                m_condition.wait(lock);

            if (this->eback() > m_begin)
            {
                this->setp(m_begin, m_begin + m_chunk_size);
                ret = this->sputc(c);
            }
        }

        m_condition.notify_one();
        return ret;
    }

    traits_type::int_type underflow() override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        traits_type::int_type ret = traits_type::eof();

        if (this->eback() != this->pbase())
        {
            if (this->egptr() < m_end)
            {
                while (this->egptr() == this->pbase() && !m_closed)
                    m_condition.wait(lock);

                if (this->egptr() != this->pbase())
                {
                    this->setg(this->eback() + m_chunk_size,
                               this->eback() + m_chunk_size,
                               this->egptr() + m_chunk_size);
                }
                else // if m_closed
                {
                    this->setg(this->pbase(), this->pbase(), this->pptr());
                }

                ret = traits_type::to_int_type(*this->gptr());
            }
            else
            {
                while (!(this->pbase() > m_begin || m_closed))
                    m_condition.wait(lock);

                if (this->pbase() > m_begin)
                {
                    this->setg(m_begin, m_begin, m_begin + m_chunk_size);
                }
                else // if m_closed
                {
                    this->setg(this->pbase(), this->pbase(), this->pptr());
                }

                ret = traits_type::to_int_type(*this->gptr());
            }
        }

        m_condition.notify_one();
        return ret;
    }
};

template<class _Elem, class _Traits = std::char_traits<_Elem>>
class basic_opipestream :
        public std::basic_ostream<_Elem, _Traits>
{
    using buffer_type = basic_pipebuf<_Elem, _Traits> ;
    buffer_type* mPbuf;

public:
    basic_opipestream(buffer_type* pBuf) :
        std::basic_ostream<_Elem, _Traits>(pBuf),
        mPbuf(pBuf)
    {
    }

    void close()
    {
        mPbuf->close();
    }
};

template<class _Elem, class _Traits = std::char_traits<_Elem>>
class basic_ipipestream :
        public std::basic_istream<_Elem, _Traits>
{
    using buffer_type = basic_pipebuf<_Elem, _Traits>;
    buffer_type* mPbuf;

public:
    basic_ipipestream(buffer_type* pBuf) :
        std::basic_istream<_Elem, _Traits>(pBuf),
        mPbuf(pBuf)
    {
    }

    void close()
    {
        mPbuf->close();
    }
};

using pipebuf     = basic_pipebuf<char>;
using opipestream = basic_opipestream<char>;
using ipipestream = basic_ipipestream<char>;
