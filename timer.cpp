#include "timer.h"

Timer::Timer() : start(boost::chrono::system_clock::now()), print_at_destruction(false) {}

Timer::Timer(const std::string &name, bool print_at_destruction) : start(boost::chrono::system_clock::now()), name(name), print_at_destruction(print_at_destruction){}

Timer::~Timer() {
    if(this->print_at_destruction)
        std::cout << *this << std::endl;
}

int Timer::ms() const {
    auto delta = boost::chrono::system_clock::now() - this->start;
    return boost::chrono::duration_cast<boost::chrono::milliseconds>(delta).count();
}

void Timer::reset() {
    start = boost::chrono::system_clock::now();
}

std::ostream & operator<<(std::ostream & os, const Timer & timer){
    os << "Timer " << timer.name << " ";
    int ms = timer.ms();

    if(ms < 10000)
        os << ms << " ms";
    else
        os << (ms/1000) << " s";

    return os;
}

