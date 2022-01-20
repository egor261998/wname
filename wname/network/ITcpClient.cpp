#include "../stdafx.h"

using wname::network::ITcpClient;
using wname::network::socket::CSocketAddress;
using wname::network::socket::ESocketState;

//==============================================================================
ITcpClient::ITcpClient(
	const std::shared_ptr<io::iocp::CIocp>& pIocp) :
	CSocketIo(pIocp)
{
	try
	{
		/** инициализируем сокет */
		_socket = socket::CSocketHandle(SOCK_STREAM, IPPROTO_TCP);
		if (!_socket.isValid())
		{
			const auto dwResult = WSAGetLastError();

			std::string strError =
				"Create of client socket failed with error: " +
				std::to_string(dwResult);

			throw std::runtime_error(strError);
		}

		bindHandle(_socket);
	}
	catch (const std::exception& ex)
	{
		pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code ITcpClient::setKeepAlive(
	const bool bValue) noexcept
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	cs::CCriticalSectionScoped lock(_csCounter);

	return _socket.setKeepAlive(bValue);
}
//==============================================================================
std::error_code ITcpClient::startAsyncSend(
	const PBYTE bufferSend,
	const DWORD dwBufferSize,
	const DWORD dwFlags)
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		if (bufferSend == nullptr || dwBufferSize == 0)
		{
			throw std::invalid_argument("bufferSend == nullptr || dwBufferSize == 0");
		}

		addAsyncIoPending();
		const auto ec = __super::startAsyncSend(
			bufferSend, dwBufferSize, dwFlags);
		if (ec)
		{
			delAsyncIoPending();
			disconnect(ec);
			return ec;
		}

		counter.release();
		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		disconnect(std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));
		throw;
	}
}
//==============================================================================
std::error_code ITcpClient::startSend(
	const PBYTE bufferSend,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	const DWORD dwFlags)
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		if (bufferSend == nullptr || dwBufferSize == 0)
		{
			throw std::invalid_argument("bufferSend == nullptr || dwBufferSize == 0");
		}

		const auto ec = __super::startSend(
			bufferSend, dwBufferSize, pdwReturnSize, dwFlags);
		if (ec)
		{
			disconnect(ec);
			return ec;
		}

		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		disconnect(std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));
		throw;
	}
}
//==============================================================================
std::error_code ITcpClient::startAsyncRecv(
	const PBYTE bufferRecv,
	const DWORD dwBufferSize,
	const DWORD dwFlags)
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		if (bufferRecv == nullptr || dwBufferSize == 0)
		{
			throw std::invalid_argument("bufferSend == nullptr || dwBufferSize == 0");
		}

		addAsyncIoPending();
		const auto ec = __super::startAsyncRecv(
			bufferRecv, dwBufferSize, dwFlags);
		if (ec)
		{
			delAsyncIoPending();
			disconnect(ec);
			return ec;
		}

		counter.release();
		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		disconnect(std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));
		throw;
	}
}
//==============================================================================
std::error_code ITcpClient::startRecv(
	const PBYTE bufferRecv,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	const DWORD dwFlags)
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		if (bufferRecv == nullptr || dwBufferSize == 0)
		{
			throw std::invalid_argument("bufferSend == nullptr || dwBufferSize == 0");
		}

		const auto ec = __super::startRecv(
			bufferRecv, dwBufferSize, pdwReturnSize, dwFlags);
		if (ec)
		{
			disconnect(ec);
			return ec;
		}

		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		disconnect(std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));
		throw;
	}
}
//==============================================================================
void ITcpClient::asyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	assert(bufferSend != nullptr);

	delAsyncIoPending();
	clientAsyncSendComplitionHandler(bufferSend, dwReturnSize, ec);
	if (ec)
		disconnect(ec);

	endOperation();
}
//==============================================================================
void ITcpClient::asyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	assert(bufferRecv != nullptr);

	delAsyncIoPending();
	clientAsyncRecvComplitionHandler(bufferRecv, dwReturnSize, ec);
	if (ec)
		disconnect(ec);

	endOperation();
}
//==============================================================================
void ITcpClient::addAsyncIoPending() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	_nAsyncIoPending++;
}
//==============================================================================
void ITcpClient::delAsyncIoPending() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	assert(_nAsyncIoPending > 0);
	_nAsyncIoPending--;
}
//==============================================================================
void ITcpClient::clientAsyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferRecv);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void ITcpClient::clientAsyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferSend);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void ITcpClient::clientConnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void ITcpClient::clientDisconnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void ITcpClient::release(
	const bool bIsWait) noexcept
{
	__super::release(false);
	/** отключаем */
	disconnect();

	/** ждем завершения всего */
	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//==============================================================================
ITcpClient::~ITcpClient()
{
	/** завершение всего */
	release(true);
}
//==============================================================================