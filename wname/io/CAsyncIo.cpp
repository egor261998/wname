#include "../stdafx.h"

using wname::io::CAsyncIo;

//==============================================================================
CAsyncIo::CAsyncIo(
	const std::shared_ptr<iocp::CIocp>& pIocp) :
	_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		throw std::invalid_argument("_pIocp == nullptr");
	}
}
//==============================================================================
void CAsyncIo::bindHandle(
	handle::CHandle hHandle)
{
	try
	{
		_pIocp->bind(hHandle);

		cs::CCriticalSectionScoped lock(_csCounter);
		_hAsyncHandle = hHandle;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
bool CAsyncIo::isBindHandle() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _hAsyncHandle.isValid();
}
//==============================================================================
void CAsyncIo::changeHandle(
	handle::CHandle hHandle)
{
	try
	{
		closeHandle();
		bindHandle(hHandle);	
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CAsyncIo::closeHandle() noexcept
{

	cs::CCriticalSectionScoped lock(_csCounter);

	/** ????????? ??? ?????????? */
	if (_hAsyncHandle.isValid())
	{
		CancelIoEx(_hAsyncHandle, nullptr);
		_hAsyncHandle.close();
	}
}
//==============================================================================
std::error_code CAsyncIo::startAsyncRead(
	const PBYTE bufferRead,
	const DWORD dwBufferSize,
	const UINT64 offset)
{
	#pragma warning (disable: 26493)
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		auto pAsyncOperation =
			_pIocp->getAsyncOperation(this, asyncReadIocpHandler);
		assert(pAsyncOperation != nullptr);

		pAsyncOperation->_buffer._p = bufferRead;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;
		*(UINT64*)&pAsyncOperation->_overlapped.Offset = offset;

		if (!ReadFile(getHandle(),
			pAsyncOperation->_buffer._p,
			pAsyncOperation->_buffer._dwSize,
			nullptr,
			&pAsyncOperation->_overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				pAsyncOperation->cancel();
				return std::error_code(dwResult, std::system_category());
			}
		}

		counter.release();
		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CAsyncIo::startRead(
	const PBYTE bufferRead,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	const UINT64 offset)
{
	#pragma warning (disable: 26493)
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::error_code ec;
		handle::CEvent hEvent;
		DWORD dwReturnSize = 0;
		auto context = std::tuple(&hEvent, &dwReturnSize, &ec);

		auto pAsyncOperation =
			_pIocp->getAsyncOperation(&context, asyncIocpHandler);
		assert(pAsyncOperation != nullptr);

		pAsyncOperation->_buffer._p = bufferRead;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;
		*(UINT64*)&pAsyncOperation->_overlapped.Offset = offset;

		if (!ReadFile(getHandle(),
			pAsyncOperation->_buffer._p,
			pAsyncOperation->_buffer._dwSize,
			nullptr,
			&pAsyncOperation->_overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				pAsyncOperation->cancel();
				return std::error_code(dwResult, std::system_category());
			}
		}

		hEvent.wait();
		_nCountReadByte += dwReturnSize;
		if (pdwReturnSize != nullptr)
		{
			*pdwReturnSize = dwReturnSize;
		}
		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CAsyncIo::startAsyncWrite(
	const PBYTE bufferWrite,
	const DWORD dwBufferSize,
	const UINT64 offset)
{
	#pragma warning (disable: 26493)
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		auto pAsyncOperation =
			_pIocp->getAsyncOperation(this, asyncWriteIocpHandler);
		assert(pAsyncOperation != nullptr);

		pAsyncOperation->_buffer._p = bufferWrite;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;
		*(UINT64*)&pAsyncOperation->_overlapped.Offset = offset;

		if (!WriteFile(getHandle(),
			pAsyncOperation->_buffer._p,
			pAsyncOperation->_buffer._dwSize,
			nullptr,
			&pAsyncOperation->_overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				pAsyncOperation->cancel();
				return std::error_code(dwResult, std::system_category());
			}
		}

		counter.release();
		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CAsyncIo::startWrite(
	const PBYTE bufferWrite,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	const UINT64 offset)
{
	#pragma warning(disable: 26493)
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::error_code ec;
		handle::CEvent hEvent;
		DWORD dwReturnSize = 0;
		auto context = std::tuple(&hEvent, &dwReturnSize, &ec);

		auto pAsyncOperation =
			_pIocp->getAsyncOperation(&context, asyncIocpHandler);
		assert(pAsyncOperation != nullptr);

		pAsyncOperation->_buffer._p = bufferWrite;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;
		*(UINT64*)&pAsyncOperation->_overlapped.Offset = offset;

		if (!WriteFile(getHandle(),
			pAsyncOperation->_buffer._p,
			pAsyncOperation->_buffer._dwSize,
			nullptr,
			&pAsyncOperation->_overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				pAsyncOperation->cancel();
				return std::error_code(dwResult, std::system_category());
			}
		}

		hEvent.wait();
		_nCountWriteByte += dwReturnSize;
		if (pdwReturnSize != nullptr)
		{
			*pdwReturnSize = dwReturnSize;
		}
		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
HANDLE CAsyncIo::getHandle() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _hAsyncHandle;
}
//==============================================================================
void CAsyncIo::asyncReadComplitionHandler(
	const PBYTE bufferRead,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferRead);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
};
//==============================================================================
void CAsyncIo::asyncWriteComplitionHandler(
	const PBYTE bufferWrite,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferWrite);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
};
//==============================================================================
void CAsyncIo::release(
	const bool bIsWait) noexcept
{
	__super::release(false);
	closeHandle();
	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//==============================================================================
void CAsyncIo::asyncReadIocpHandler(
	iocp::CAsyncOperation* const pAsyncOperation) noexcept
{
	/** ??????????? ??????????? ???????? */
	assert(pAsyncOperation != nullptr);

	/** ???????? ???????? */
	assert(pAsyncOperation->_pCompletionRoutineContext != nullptr);
	const auto _this = static_cast<CAsyncIo*>(
		pAsyncOperation->_pCompletionRoutineContext);
	assert(_this != nullptr);

	_this->_nCountReadByte += pAsyncOperation->_dwReturnSize;

	/** ?????????? ???????????? ?????? */
	_this->asyncReadComplitionHandler(
		pAsyncOperation->_buffer._p,
		pAsyncOperation->_dwReturnSize,
		pAsyncOperation->_ec);

	_this->endOperation();
}
//==============================================================================
void CAsyncIo::asyncWriteIocpHandler(
	iocp::CAsyncOperation* const pAsyncOperation) noexcept
{
	/** ??????????? ??????????? ???????? */
	assert(pAsyncOperation != nullptr);

	/** ???????? ???????? ??????? */
	assert(pAsyncOperation->_pCompletionRoutineContext != nullptr);
	const auto _this = static_cast<CAsyncIo*>(
		pAsyncOperation->_pCompletionRoutineContext);
	assert(_this != nullptr);

	_this->_nCountWriteByte += pAsyncOperation->_dwReturnSize;

	/** ?????????? ??????????? ?????? */
	_this->asyncWriteComplitionHandler(
		pAsyncOperation->_buffer._p,
		pAsyncOperation->_dwReturnSize,
		pAsyncOperation->_ec);

	_this->endOperation();
}
//==============================================================================
void CAsyncIo::asyncIocpHandler(
	iocp::CAsyncOperation* const pAsyncOperation) noexcept
{
	/** ??????????? ??????????? ???????? */
	assert(pAsyncOperation != nullptr);

	/** ???????? ???????? ??????? */
	assert(pAsyncOperation->_pCompletionRoutineContext != nullptr);
	const auto contex = *static_cast<
		std::tuple<handle::CEvent*, PDWORD, std::error_code*>*>(
		pAsyncOperation->_pCompletionRoutineContext);

	if (std::get<1>(contex) != nullptr)
	{
		/** ???????????? ?????? ???????? */
		*std::get<1>(contex) = pAsyncOperation->_dwReturnSize;
	}

	if (std::get<2>(contex) != nullptr)
	{
		/** ?????? ???????? */
		*std::get<2>(contex) = pAsyncOperation->_ec;
	}

	if (const auto ec = std::get<0>(contex)->notify(); ec)
	{
		pAsyncOperation->_pIocp->log(
			logger::EMessageType::critical, 
			L"Notify failed",
			ec);
	}
}
//==============================================================================
CAsyncIo::~CAsyncIo()
{
	release(true);
}
//==============================================================================