#include "../stdafx.h"

using wname::cs::CCriticalSection;

//==============================================================================
CCriticalSection::CCriticalSection() noexcept
{
	InitializeCriticalSection(this);
}
//==============================================================================
_Acquires_lock_(*this)
void CCriticalSection::lock() noexcept
{
	EnterCriticalSection(this);
}
//==============================================================================
_Acquires_lock_(*this)
_Success_(return != false)
bool CCriticalSection::tryLock() noexcept
{
	return TryEnterCriticalSection(this);
}
//==============================================================================
_Releases_lock_(*this)
void CCriticalSection::unLock() noexcept
{
	LeaveCriticalSection(this);
}
//==============================================================================
CCriticalSection::~CCriticalSection()
{
	DeleteCriticalSection(this);
}
//==============================================================================