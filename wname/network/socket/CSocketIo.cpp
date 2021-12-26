#include "../../stdafx.h"

using CSocketIoPrefix = wname::network::socket::CSocketIo;

//==============================================================================
CSocketIoPrefix::CSocketIo(
	const std::shared_ptr<io::iocp::CIocp>& pIocp)
	:CAsyncIo(pIocp)
{ 
	/** инициализация */
	WSADATA wsaData;
	if (const auto er = WSAStartup(MAKEWORD(2, 2), &wsaData); er != ERROR_SUCCESS)
		throw std::runtime_error("Error at WSAStartup: " + std::to_string(er));
}
//==============================================================================
std::error_code CSocketIoPrefix::startAsyncRecv(
	const PBYTE bufferRecv,
	const DWORD dwBufferSize,
	DWORD dwFlags)
{
	#pragma warning (disable: 26493)
	misc::CCounterScoped counter(*this, 2);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		auto pAsyncOperation =
			_pIocp->getAsyncOperation(this, asyncReadIocpHandler);
		assert(pAsyncOperation != nullptr);

		pAsyncOperation->_buffer._p = bufferRecv;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;

		{
			/** необходимая синхронизация для корректного отключения
			во время выполняемых асинхронных операций */
			cs::CCriticalSectionScoped lock(_csCounter);
			_nCountIoOperation++;
		}
		
		if (WSARecv((SOCKET)getHandle(),
			&pAsyncOperation->_wsaBuffer,
			1,
			nullptr,
			&dwFlags,
			&pAsyncOperation->_overlapped,
			nullptr) == SOCKET_ERROR)
		{
			const auto dwResult = WSAGetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				cs::CCriticalSectionScoped lock(_csCounter);
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
std::error_code CSocketIoPrefix::startRecv(
	const PBYTE bufferRecv,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	DWORD dwFlags)
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

		pAsyncOperation->_buffer._p = bufferRecv;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;

		if (WSARecv((SOCKET)getHandle(),
			&pAsyncOperation->_wsaBuffer,
			1,
			nullptr,
			&dwFlags,
			&pAsyncOperation->_overlapped,
			nullptr) == SOCKET_ERROR)
		{
			const auto dwResultPending = WSAGetLastError();

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

		return !ec && dwReturnSize == 0 ?
			std::error_code(ERROR_OPERATION_ABORTED, std::system_category()) : ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CSocketIoPrefix::startAsyncSend(
	const PBYTE bufferSend,
	const DWORD dwBufferSize,
	const DWORD dwFlags)
{
	#pragma warning (disable: 26493)
	misc::CCounterScoped counter(*this, 2);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		auto pAsyncOperation =
			_pIocp->getAsyncOperation(this, asyncWriteIocpHandler);
		assert(pAsyncOperation != nullptr);

		pAsyncOperation->_buffer._p = bufferSend;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;

		{
			/** необходимая синхронизация для корректного отключения
			во время выполняемых асинхронных операций */
			cs::CCriticalSectionScoped lock(_csCounter);
			_nCountIoOperation++;
		}

		if (WSASend((SOCKET)getHandle(),
			&pAsyncOperation->_wsaBuffer,
			1,
			nullptr,
			dwFlags,
			&pAsyncOperation->_overlapped,
			nullptr) == SOCKET_ERROR)
		{
			const auto dwResult = WSAGetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				cs::CCriticalSectionScoped lock(_csCounter);
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
std::error_code CSocketIoPrefix::startSend(
	const PBYTE bufferSend,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	const DWORD dwFlags)
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

		pAsyncOperation->_buffer._p = bufferSend;
		pAsyncOperation->_buffer._dwSize = dwBufferSize;

		if (WSASend((SOCKET)getHandle(),
			&pAsyncOperation->_wsaBuffer,
			1,
			nullptr,
			dwFlags,
			&pAsyncOperation->_overlapped,
			nullptr) == SOCKET_ERROR)
		{
			const auto dwResultPending = WSAGetLastError();

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
		return !ec && dwReturnSize == 0 ?
			std::error_code(ERROR_OPERATION_ABORTED, std::system_category()) : ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CSocketIoPrefix::release() noexcept
{
	__super::release();
}
//==============================================================================
void CSocketIoPrefix::asyncReadComplitionHandler(
	const PBYTE bufferRead,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	const auto ecComplition =
		!ec && dwReturnSize == 0 ?
		std::error_code(ERROR_OPERATION_ABORTED, std::system_category()) : ec;

	asyncRecvComplitionHandler(bufferRead, dwReturnSize, ecComplition);

	endOperation();
}
//==============================================================================
void CSocketIoPrefix::asyncWriteComplitionHandler(
	const PBYTE bufferWrite,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	const auto ecComplition =
		!ec && dwReturnSize == 0 ?
		std::error_code(ERROR_OPERATION_ABORTED, std::system_category()) : ec;

	asyncSendComplitionHandler(bufferWrite, dwReturnSize, ecComplition);

	endOperation();
}
//==============================================================================
void CSocketIoPrefix::asyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferRecv);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CSocketIoPrefix::asyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferSend);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
CSocketIoPrefix::~CSocketIo()
{
	release();
	WSACleanup();
}
//==============================================================================
