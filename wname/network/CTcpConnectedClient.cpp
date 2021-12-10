#include "../stdafx.h"

using CTcpConnectedClientPrefix = wname::network::CTcpServer::CTcpConnectedClient;
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
		_socket = socket::CSocketHandle(SOCK_STREAM, IPPROTO_TCP);

		if (!_socket.isValid())
		{
			std::string strError = "Create of client socket failed with error: " +
				std::to_string(WSAGetLastError());

			throw std::runtime_error(strError);
		}

		initialize(_socket);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTcpConnectedClientPrefix::startAsyncSend(
	const PBYTE bufferSend,
	const DWORD dwBufferSize,
	const DWORD dwFlags)
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		if (bufferSend == nullptr || dwBufferSize == 0)
		{
			throw std::invalid_argument("bufferSend == nullptr || dwBufferSize == 0");
		}

		const auto ec = __super::startAsyncSend(
			bufferSend, dwBufferSize, dwFlags);
		if (ec)
		{
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
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

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
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		if (bufferRecv == nullptr || dwBufferSize == 0)
		{
			throw std::invalid_argument("bufferSend == nullptr || dwBufferSize == 0");
		}

		const auto ec = __super::startAsyncRecv(
			bufferRecv, dwBufferSize, dwFlags);
		if (ec)
		{
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
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

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
	cs::CCriticalSectionScoped lock(_csCounter);

	switch (_eSocketState)
	{
	case ESocketStatePrefix::disconnected:
		break;
	case ESocketStatePrefix::disconnecting:
	{
		/** еще не все операции отработали */
		if (_nCountIoOperation > 0)
			return;

		_eSocketState = ESocketStatePrefix::disconnected;
		_socket.close();

		/** просто закрываемся */
		_pParent->clientDisconnected(this, _ec);
		_pParent->removeClient(this);

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
		disconnect();
		break;
	}
	default:
		break;
	}
}
//==============================================================================
void CTcpConnectedClientPrefix::asyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	assert(bufferSend != nullptr);

	if (ec)
		disconnect(ec);

	_pParent->clientAsyncSendComplite(
		this, bufferSend, dwReturnSize, ec);

	endOperation();
}
//==============================================================================
void CTcpConnectedClientPrefix::asyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	assert(bufferRecv);

	if (ec)
		disconnect(ec);

	_pParent->clientAsyncRecvComplite(
		this, bufferRecv, dwReturnSize, ec);

	endOperation();
}
//==============================================================================
CTcpConnectedClientPrefix::~CTcpConnectedClient()
{
	/** отключаем */
	disconnect();

	/** ждем завершения всего */
	release();
}
//==============================================================================