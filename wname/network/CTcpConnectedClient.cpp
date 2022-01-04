#include "../stdafx.h"

using CTcpConnectedClientPrefix = wname::network::CTcpServer::CTcpConnectedClient;
using CSocketAddressPrefix = wname::network::socket::CSocketAddress;
using ESocketStatePrefix = wname::network::socket::ESocketState;

//==============================================================================
CTcpConnectedClientPrefix::CTcpConnectedClient(
	CTcpServer* pParent)
:CSocketIo(pParent->_pIocp),
_pParent(pParent)
{
	if (_pParent == nullptr)
		throw std::logic_error("_pParent == nullptr");

	try
	{
		misc::CCounterScoped counter(*_pParent);
		if (!counter.isStartOperation())
			throw std::logic_error("Server is not start");

		_socket = socket::CSocketHandle(SOCK_STREAM, IPPROTO_TCP);

		if (!_socket.isValid())
		{
			std::string strError = "Create of client socket failed with error: " +
				std::to_string(WSAGetLastError());

			throw std::runtime_error(strError);
		}

		bindHandle(_socket);
		counter.release();
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTcpConnectedClientPrefix::setKeepAlive(
	const bool bValue) noexcept
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	cs::CCriticalSectionScoped lock(_csCounter);

	return _socket.setKeepAlive(bValue);
}
//==============================================================================
CSocketAddressPrefix CTcpConnectedClientPrefix::getClientAddress() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _localAddress;
}
//==============================================================================
CSocketAddressPrefix CTcpConnectedClientPrefix::getServerAddress() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _remotelAddress;
}
//==============================================================================
ESocketStatePrefix CTcpConnectedClientPrefix::getState() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _eSocketState;
}
//==============================================================================
std::error_code CTcpConnectedClientPrefix::startAsyncSend(
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
std::error_code CTcpConnectedClientPrefix::startSend(
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
std::error_code CTcpConnectedClientPrefix::startAsyncRecv(
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
std::error_code CTcpConnectedClientPrefix::startRecv(
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
void CTcpConnectedClientPrefix::disconnect(
	const std::error_code ec) noexcept
{
	/** счетчик операций дл€ корректного завершени€ контекста */
	misc::CCounterScoped counter(*this);

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
			_socket.close();

			/** просто закрываемс€ */
			ecDisconnected = _ec;
			bIsDisconnected = true;

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
		_pParent->removeClient(this);
	}
}
//==============================================================================
void CTcpConnectedClientPrefix::asyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	assert(bufferSend != nullptr);

	delAsyncIoPending();
	clientAsyncSendComplitionHandler(
		bufferSend, dwReturnSize, ec);
	if (ec)
		disconnect(ec);

	endOperation();
}
//==============================================================================
void CTcpConnectedClientPrefix::asyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	assert(bufferRecv);

	delAsyncIoPending();
	clientAsyncRecvComplitionHandler(
		bufferRecv, dwReturnSize, ec);
	if (ec)
		disconnect(ec);

	endOperation();
}
//==============================================================================
void CTcpConnectedClientPrefix::addAsyncIoPending() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	_nAsyncIoPending++;
}
//==============================================================================
void CTcpConnectedClientPrefix::delAsyncIoPending() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	assert(_nAsyncIoPending > 0);
	_nAsyncIoPending--;
}
//==============================================================================
void CTcpConnectedClientPrefix::clientAsyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferRecv);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpConnectedClientPrefix::clientAsyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferSend);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpConnectedClientPrefix::clientConnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpConnectedClientPrefix::clientDisconnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpConnectedClientPrefix::release() noexcept
{
	/** отключаем */
	disconnect();

	/** ждем завершени€ всего */
	__super::release();
}
//==============================================================================
CTcpConnectedClientPrefix::~CTcpConnectedClient()
{
	/** завершение всего */
	release();

	/** снимаем ссылку с сервера */
	_pParent->endOperation();
}
//==============================================================================