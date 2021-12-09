#include "../stdafx.h"

using CCounterScopedPrefix = wname::misc::CCounterScoped;

//==============================================================================
CCounterScopedPrefix::CCounterScoped(
	CCounter& counter,
	const DWORD dwCount) noexcept
	: _counter(counter), _dwCount(dwCount)
{
	for (DWORD i = 0; i < _dwCount; i++)
	{
		_bIsStartOperation = _counter.startOperation();
		if (!_bIsStartOperation)
		{
			/** отменяем предыдущие */
			for (DWORD j = 0; j < i; j++)
			{
				_counter.endOperation();
			}
		}
	}
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
	{
		for (DWORD i = 0; i < _dwCount; i++)
			_counter.endOperation();
	}				
}
//==============================================================================