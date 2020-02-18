#ifndef _CRITICAL_SECTION_H_
#define _CRITICAL_SECTION_H_

#include <Windows.h>

class CriticalSection {
public:
    explicit CriticalSection();
    ~CriticalSection();

    void lock();
    void unlock();
    bool try_enter();

private:
    CRITICAL_SECTION critical_section_;
};


#endif