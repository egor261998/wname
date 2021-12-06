#include "../../stdafx.h"

using CAsyncOperationPrefix = wname::io::iocp::CIocp::CAsyncOperation;

//==============================================================================
CAsyncOperationPrefix::CAsyncOperation(
	CIocp* const pIocp,
	const PVOID pCompletionRoutineContext,
	const FAsyncCompletion fComplitionRoutine)
:_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		throw std::logic_error("_pIocp == nullptr");
	}

	_pCompletionRoutineContext = pCompletionRoutineContext;
	_fComplitionRoutine = fComplitionRoutine;
}
//==============================================================================
void CAsyncOperationPrefix::CAsyncOperation::cancel() noexcept
{
	const auto pIocp = _pIocp;
	assert(pIocp != nullptr);

	_pIocp->_listFreeAsyncOperation.push(this);
	pIocp->endOperation();
}
//==============================================================================