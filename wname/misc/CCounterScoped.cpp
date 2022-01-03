#include "../stdafx.h"

using CCounterScopedPrefix = wname::misc::CCounterScoped;

//==============================================================================
CCounterScopedPrefix::CCounterScoped(
	CCounter& counter,
	const size_t nCount) noexcept
:_counter(counter)
{
	if (_counter.startOperation(nCount))
		_nCountStart = nCount;
}
//==============================================================================
bool CCounterScopedPrefix::isStartOperation() const noexcept
{
	return _nCountStart > 0;
}
//==============================================================================
void CCounterScopedPrefix::release() noexcept
{
	_nCountStart = 0;
}
//==============================================================================
CCounterScopedPrefix::~CCounterScoped()
{
	if(_nCountStart > 0)
		_counter.endOperation(_nCountStart);
}
//==============================================================================