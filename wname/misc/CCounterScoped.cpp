#include "../stdafx.h"

using CCounterScopedPrefix = wname::misc::CCounterScoped;

//==============================================================================
	CCounterScopedPrefix::CCounterScoped(
		CCounter& counter)
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
		try
		{
			if (_bIsStartOperation)
				_counter.endOperation();
		}
		catch (const std::exception& ex)
		{
			UNREFERENCED_PARAMETER(ex);

			/** тут смерть */
			assert(false);
		}		
	}
//==============================================================================