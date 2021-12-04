#include "../stdafx.h"

using CCounterScopedPrefix = wname::misc::CCounterScoped;

//==============================================================================
	CCounterScopedPrefix::CCounterScoped(
		CCounter& counter) noexcept
		: _counter(counter)
	{
		_bIsStartOperation = _counter.startOperation();
	}
//==============================================================================
	bool CCounterScopedPrefix::isStartOperation() const noexcept
	{
		return _bIsStartOperation;
	}
//==============================================================================
	void CCounterScopedPrefix::release() noexcept
	{
		_bIsStartOperation = false;
	}
//==============================================================================
	CCounterScopedPrefix::~CCounterScoped()
	{
		if (_bIsStartOperation)
			_counter.endOperation();		
	}
//==============================================================================