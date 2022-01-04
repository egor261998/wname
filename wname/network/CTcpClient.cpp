#include "../stdafx.h"

using CTcpClientPrefix = wname::network::CTcpClient;
using CSocketAddressPrefix = wname::network::socket::CSocketAddress;
using ESocketStatePrefix = wname::network::socket::ESocketState;

//==============================================================================
CTcpClientPrefix::CTcpClient(
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<io::iocp::CIocp>& pIocp)
:CSocketIo(pIocp)
{
	try
	{
		_socketAddress = socket::CSocketAddress(strIp, wPort);

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
std::error_code CTcpClientPrefix::connect() noexcept
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	std::error_code ec;

	{
		/** отдельная область видимости для удобства блокировки */
		cs::CCriticalSectionScoped lock(_csCounter);

		if (_eSocketState == ESocketStatePrefix::disconnected)
		{
			/** действуем */
			_eSocketState = ESocketStatePrefix::connecting;

			/** пробуем соединиться */
			if (::connect(_socket, _socketAddress, _socketAddress.size()) ==
				SOCKET_ERROR)
			{
				ec = std::error_code(WSAGetLastError(), std::system_category());
				_eSocketState = ESocketStatePrefix::disconnected;
			}
			else
			{
				_eSocketState = ESocketStatePrefix::connected;
			}
		}
		else
		{
			/** еще рано */
			ec = std::error_code(ERROR_INVALID_STATE, std::system_category());
		}
	}

	clientConnected(ec);
	return ec;
}
//==============================================================================
std::error_code CTcpClientPrefix::setKeepAlive(
	const bool bValue) noexcept
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	cs::CCriticalSectionScoped lock(_csCounter);

	return _socket.setKeepAlive(bValue);
}
//==============================================================================
std::error_code CTcpClientPrefix::startAsyncSend(
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
std::error_code CTcpClientPrefix::startSend(
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
std::error_code CTcpClientPrefix::startAsyncRecv(
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
std::error_code CTcpClientPrefix::startRecv(
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
void CTcpClientPrefix::disconnect(
	const std::error_code ec) noexcept
{
	bool bIsRepeatDisconnect = false;
	bool bIsDisconnected = false;
	std::error_code ecDisconnected;

	{
		cs::CCriticalSectionScoped lock(_csCounter);

		switch (_eSocketState)
		{
		case ESocketStatePrefix::disconnected:
			break;
		case ESocketStatePrefix::disconnecting:
		{
			/** еще не все операции отработали */
			if (_nAsyncIoPending > 0)
				break;

			_eSocketState = ESocketStatePrefix::disconnected;
			if (!isInitialize())
			{
				/** просто закрываемся */
				ecDisconnected = std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
				bIsDisconnected = true;
				break;
			}

			try
			{
				/** пересоздаем сокет */
				_socket = socket::CSocketHandle(SOCK_STREAM, IPPROTO_TCP);

				if (!_socket.isValid())
				{
					ecDisconnected = std::error_code(WSAGetLastError(), std::system_category());
					bIsDisconnected = true;
					break;
				}

				/** пробуем пересоздать сокет */
				changeHandle(_socket);

				/** скидываем именно ту ошибку, которая инициировала отключение */
				ecDisconnected = _ec;
				bIsDisconnected = true;
			}
			catch (const std::exception& ex)
			{
				/** просто закрываемся */
				_pIocp->log(logger::EMessageType::warning, ex);

				ecDisconnected = std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
				bIsDisconnected = true;
			}

			break;
		}
		case ESocketStatePrefix::connecting:
			break;
		case ESocketStatePrefix::connected:
		{
			_eSocketState = ESocketStatePrefix::disconnecting;
			_ec = ec;

			try
			{
				if (!_socket.getDisconnectex()(_socket, nullptr, 0, 0))
				{
					if (!_ec)
						_ec = std::error_code(WSAGetLastError(), std::system_category());
				}
			}
			catch (const std::exception& ex)
			{
				_pIocp->log(logger::EMessageType::warning, ex);
			}

			shutdown(_socket, SD_BOTH);
			CancelIoEx(_socket, nullptr);
			bIsRepeatDisconnect = true;
			break;
		}
		default:
			break;
		}
	}

	if (bIsRepeatDisconnect)
	{
		/** повторное отключение */
		disconnect();
	}

	if (bIsDisconnected)
	{
		/** отключаем клиента */
		clientDisconnected(ecDisconnected);
	}
}
//==============================================================================
CSocketAddressPrefix CTcpClientPrefix::getAddress() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _socketAddress;
}
//==============================================================================
ESocketStatePrefix CTcpClientPrefix::getState() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _eSocketState;
}
//==============================================================================
void CTcpClientPrefix::asyncSendComplitionHandler(
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
void CTcpClientPrefix::asyncRecvComplitionHandler(
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
void CTcpClientPrefix::addAsyncIoPending() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	_nAsyncIoPending++;
}
//==============================================================================
void CTcpClientPrefix::delAsyncIoPending() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	assert(_nAsyncIoPending > 0);
	_nAsyncIoPending--;
}
//==============================================================================
void CTcpClientPrefix::clientAsyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferRecv);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpClientPrefix::clientAsyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferSend);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpClientPrefix::clientConnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpClientPrefix::clientDisconnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpClientPrefix::release() noexcept
{
	/** отключаем */
	disconnect();

	/** ждем завершения всего */
	__super::release();
}
//==============================================================================
CTcpClientPrefix::~CTcpClient()
{
	/** завершение всего */
	release();
}
//==============================================================================