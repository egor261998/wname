#include "../stdafx.h"

using CCriticalSectionPrefix = wname::cs::CCriticalSection;

//==============================================================================
	CCriticalSectionPrefix::CCriticalSection() noexcept
	{
		InitializeCriticalSection(this);
	}
//==============================================================================
	_Acquires_lock_(*this)
	void CCriticalSectionPrefix::lock() noexcept
	{
		EnterCriticalSection(this);
	}
//==============================================================================
	_Acquires_lock_(*this)
	_Success_(return != false)
	bool CCriticalSectionPrefix::tryLock() noexcept
	{
		return TryEnterCriticalSection(this);
	}
//==============================================================================
	_Releases_lock_(*this)
	void CCriticalSectionPrefix::unlock() noexcept
	{
		LeaveCriticalSection(this);
	}
//==============================================================================
	CCriticalSectionPrefix::~CCriticalSection()
	{
		DeleteCriticalSection(this);
	}
//==============================================================================