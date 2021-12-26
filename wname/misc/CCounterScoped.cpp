#include "../stdafx.h"

using CCounterScopedPrefix = wname::misc::CCounterScoped;

//==============================================================================
CCounterScopedPrefix::CCounterScoped(
	CCounter& counter,
	const DWORD dwCount) noexcept
:_counter(counter)
{
	for (_dwIsStartOperation = 0; _dwIsStartOperation < dwCount; _dwIsStartOperation++)
	{
		if(!_counter.startOperation())
		{
			break;
		}
	}

	if (_dwIsStartOperation != dwCount)
	{
		/** отменяем */
		closeOperation(true);
	}
}
//==============================================================================
bool CCounterScopedPrefix::isStartOperation() const noexcept
{
	return _dwIsStartOperation > 0;
}
//==============================================================================
void CCounterScopedPrefix::release() noexcept
{
	closeOperation(false);
}
//==============================================================================
void CCounterScopedPrefix::closeOperation(
	const bool bIsEndOpertaion) noexcept
{
	if (bIsEndOpertaion)
	{
		/** завершаем все операции */
		for (DWORD i = 0; i < _dwIsStartOperation; i++)
		{
			_counter.endOperation();
		}
	}
	_dwIsStartOperation = 0;
}
//==============================================================================
CCounterScopedPrefix::~CCounterScoped()
{
	closeOperation(true);
}
//==============================================================================