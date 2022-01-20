#include "../stdafx.h"

using wname::cs::CCriticalSectionScoped;

//==============================================================================
_Acquires_lock_(_cs)
CCriticalSectionScoped::CCriticalSectionScoped(
	CCriticalSection& cs) noexcept :
	_cs(cs)
{
	_cs.lock();
}
//==============================================================================
_Releases_lock_(_cs)
CCriticalSectionScoped::~CCriticalSectionScoped()
{
	_cs.unLock();
}
//==============================================================================