#include "../stdafx.h"

using CAsyncIoPrefix = wname::io::CAsyncIo;

//==============================================================================
CAsyncIoPrefix::CAsyncIo(
	const std::shared_ptr<iocp::CIocp>& pIocp)
	:_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		throw std::invalid_argument("_pIocp == nullptr");
	}
}
//==============================================================================
void CAsyncIoPrefix::initialize(
	handle::CHandle hHandle)
{
	try
	{
		if (!hHandle.isValid())
		{
			throw std::invalid_argument("hHandle == INVALID_HANDLE");
		}

		cs::CCriticalSectionScoped lock(_csCounter);

		/** уже инициализированно */
		if (isInitialize())
		{
			throw std::logic_error("Already initialize");
		}

		_pIocp->bind(hHandle);
		__super::initialize();

		_hAsyncHandle = hHandle;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CAsyncIoPrefix::changeHandle(
	handle::CHandle hHandle)
{
	try
	{
		if (!hHandle.isValid())
		{
			throw std::invalid_argument("hHandle == INVALID_HANDLE");
		}

		cs::CCriticalSectionScoped lock(_csCounter);

		/** закрываем все предыдущее */
		CancelIoEx(_hAsyncHandle, nullptr);
		_hAsyncHandle.close();

		/** открываем по новой */
		_hAsyncHandle = hHandle;

		/** подвязываем к механизму ввода/вывода */
		_pIocp->bind(_hAsyncHandle);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CAsyncIoPrefix::release() noexcept
{
	{
		/** отдельная область видимости для синхронизации */
		cs::CCriticalSectionScoped lock(_csCounter);
		CancelIoEx(_hAsyncHandle, nullptr);
		_hAsyncHandle.close();
	}

	__super::release();
}
//==============================================================================
std::error_code CAsyncIoPrefix::startAsyncRead(
	const PBYTE bufferRead,
	const DWORD dwBufferSize,
	const UINT64 offset)
{
	#pragma warning (disable: 26493)
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		auto pAsyncOperation =
			_pIocp->getAsyncOperation(this, asyncReadIocpHandler);
		assert(pAsyncOperation != nullptr);

		pAsyncOperation->_buffer._p = bufferRead;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;
		*(UINT64*)&pAsyncOperation->_overlapped.Offset = offset;

		/** необходимая синхронизация для корректного отключения
			во время выполняемых асинхронных операций */
		cs::CCriticalSectionScoped lock(_csCounter);
		_nCountIoOperation++;

		if (!ReadFile(getHandle(),
			pAsyncOperation->_buffer._p,
			pAsyncOperation->_buffer._dwSize,
			nullptr,
			&pAsyncOperation->_overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				_nCountIoOperation--;
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
std::error_code CAsyncIoPrefix::startRead(
	const PBYTE bufferRead,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	const UINT64 offset)
{
	#pragma warning (disable: 26493)
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		std::error_code ec;
		handle::CEvent hEvent;
		DWORD dwReturnSize = 0;
		hEvent.initialize();
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
			const auto dwResultPending = GetLastError();

			if (dwResultPending != ERROR_IO_PENDING)
			{
				pAsyncOperation->cancel();
				return std::error_code(dwResultPending, std::system_category());
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
std::error_code CAsyncIoPrefix::startAsyncWrite(
	const PBYTE bufferWrite,
	const DWORD dwBufferSize,
	const UINT64 offset)
{
	#pragma warning (disable: 26493)
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		auto pAsyncOperation =
			_pIocp->getAsyncOperation(this, asyncWriteIocpHandler);
		assert(pAsyncOperation != nullptr);

		pAsyncOperation->_buffer._p = bufferWrite;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;
		*(UINT64*)&pAsyncOperation->_overlapped.Offset = offset;

		/** необходимая синхронизация для корректного отключения
			во время выполняемых асинхронных операций */
		cs::CCriticalSectionScoped lock(_csCounter);
		_nCountIoOperation++;

		if (!WriteFile(getHandle(),
			pAsyncOperation->_buffer._p,
			pAsyncOperation->_buffer._dwSize,
			nullptr,
			&pAsyncOperation->_overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				_nCountIoOperation--;
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
std::error_code CAsyncIoPrefix::startWrite(
	const PBYTE bufferWrite,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	const UINT64 offset)
{
	#pragma warning(disable: 26493)
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		std::error_code ec;
		handle::CEvent hEvent;
		DWORD dwReturnSize = 0;
		hEvent.initialize();
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
			const auto dwResultPending = GetLastError();

			if (dwResultPending != ERROR_IO_PENDING)
			{
				pAsyncOperation->cancel();
				return std::error_code(dwResultPending, std::system_category());
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
HANDLE CAsyncIoPrefix::getHandle() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _hAsyncHandle;
}
//==============================================================================
CAsyncIoPrefix::~CAsyncIo()
{
	release();
}
//==============================================================================
void CAsyncIoPrefix::asyncReadComplitionHandler(
	const PBYTE bufferRead,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferRead);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
};
//==============================================================================
void CAsyncIoPrefix::asyncWriteComplitionHandler(
	const PBYTE bufferWrite,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferWrite);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
};
//==============================================================================
void CAsyncIoPrefix::asyncReadIocpHandler(
	iocp::CAsyncOperation* const pAsyncOperation) noexcept
{
	/** отсутствует асинхронная операция */
	assert(pAsyncOperation != nullptr);

	/** получаем контекст */
	assert(pAsyncOperation->_pCompletionRoutineContext != nullptr);
	const auto _this = static_cast<CAsyncIo*>(
		pAsyncOperation->_pCompletionRoutineContext);
	assert(_this != nullptr);

	_this->_nCountReadByte += pAsyncOperation->_dwReturnSize;
	_this->_nCountIoOperation--;

	/** обработчик асинхронного чтения */
	_this->asyncReadComplitionHandler(
		pAsyncOperation->_buffer._p,
		pAsyncOperation->_dwReturnSize,
		pAsyncOperation->_ec);

	_this->endOperation();
}
//==============================================================================
void CAsyncIoPrefix::asyncWriteIocpHandler(
	iocp::CAsyncOperation* const pAsyncOperation) noexcept
{
	/** отсутствует асинхронная операция */
	assert(pAsyncOperation != nullptr);

	/** получаем контекст клиента */
	assert(pAsyncOperation->_pCompletionRoutineContext != nullptr);
	const auto _this = static_cast<CAsyncIo*>(
		pAsyncOperation->_pCompletionRoutineContext);
	assert(_this != nullptr);

	_this->_nCountWriteByte += pAsyncOperation->_dwReturnSize;
	_this->_nCountIoOperation--;

	/** обработчик асинхронной записи */
	_this->asyncWriteComplitionHandler(
		pAsyncOperation->_buffer._p,
		pAsyncOperation->_dwReturnSize,
		pAsyncOperation->_ec);

	_this->endOperation();
}
//==============================================================================
void CAsyncIoPrefix::asyncIocpHandler(
	iocp::CAsyncOperation* const pAsyncOperation) noexcept
{
	/** отсутствует асинхронная операция */
	assert(pAsyncOperation != nullptr);

	/** получаем контекст клиента */
	assert(pAsyncOperation->_pCompletionRoutineContext != nullptr);
	const auto contex = *static_cast<
		std::tuple<handle::CEvent*, PDWORD, std::error_code*>*>(
		pAsyncOperation->_pCompletionRoutineContext);

	if (std::get<1>(contex) != nullptr)
	{
		/** возвращенный размер операции */
		*std::get<1>(contex) = pAsyncOperation->_dwReturnSize;
	}

	if (std::get<2>(contex) != nullptr)
	{
		/** ошибка операции */
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