#include "../../stdafx.h"

using wname::io::iocp::CIocp;

//==============================================================================
CIocp::CAsyncOperation::CAsyncOperation(
	CIocp* const pIocp,
	const PVOID pCompletionRoutineContext,
	const FAsyncCompletion fComplitionRoutine) :
	_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		throw std::logic_error("_pIocp == nullptr");
	}

	_pCompletionRoutineContext = pCompletionRoutineContext;
	_fComplitionRoutine = fComplitionRoutine;
}
//==============================================================================
void CIocp::CAsyncOperation::cancel() noexcept
{
	const auto pIocp = _pIocp;
	assert(pIocp != nullptr);

	_pIocp->_listFreeAsyncOperation.push(this);
	pIocp->endOperation();
}
//==============================================================================