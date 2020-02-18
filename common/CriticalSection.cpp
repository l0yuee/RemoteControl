#include "CriticalSection.h"

CriticalSection::CriticalSection()
{
    ::InitializeCriticalSection(&critical_section_);
}

CriticalSection::~CriticalSection()
{
    ::DeleteCriticalSection(&critical_section_);
}

void CriticalSection::lock()
{
    ::EnterCriticalSection(&critical_section_);
}

void CriticalSection::unlock()
{
    ::LeaveCriticalSection(&critical_section_);
}

bool CriticalSection::try_enter()
{
    return ::TryEnterCriticalSection(&critical_section_);
}
