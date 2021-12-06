#include "../stdafx.h"

using CCriticalSectionScopedPrefix = wname::cs::CCriticalSectionScoped;

//==============================================================================
_Acquires_lock_(_cs)
CCriticalSectionScopedPrefix::CCriticalSectionScoped(
	CCriticalSection& cs) noexcept
:_cs(cs)
{
	_cs.lock();
}
//==============================================================================
_Releases_lock_(_cs)
CCriticalSectionScopedPrefix::~CCriticalSectionScoped()
{
	_cs.unlock();
}
//==============================================================================