#include "../stdafx.h"

using CTcpClientPrefix = wname::network::CTcpClient;
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

		/** �������������� ����� */
		_socket = socket::CSocketHandle(SOCK_STREAM, IPPROTO_TCP);
		if (!_socket.isValid())
		{
			const auto dwResult = WSAGetLastError();

			std::string strError =
				"Create of client socket failed with error: " +
				std::to_string(dwResult);

			throw std::runtime_error(strError);
		}
		
		initialize(_socket);
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
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	std::error_code ec;

	{
		/** ��������� ������� ��������� ��� �������� ���������� */
		cs::CCriticalSectionScoped lock(_csCounter);

		if (_eSocketState == ESocketStatePrefix::disconnected)
		{
			/** ��������� */
			_eSocketState = ESocketStatePrefix::connecting;

			/** ������� ����������� */
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
			/** ��� ���� */
			ec = std::error_code(ERROR_INVALID_STATE, std::system_category());
		}
	}

	clientConnected(ec);
	return ec;
}
//==============================================================================
std::error_code CTcpClientPrefix::startAsyncSend(
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
std::error_code CTcpClientPrefix::startSend(
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
std::error_code CTcpClientPrefix::startAsyncRecv(
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
std::error_code CTcpClientPrefix::startRecv(
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
void CTcpClientPrefix::disconnect(
	const std::error_code ec) noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	switch (_eSocketState)
	{
	case ESocketStatePrefix::disconnected:
		break;
	case ESocketStatePrefix::disconnecting:
	{
		/** ��� �� ��� �������� ���������� */
		if (_nCountIoOperation > 0)
			return;

		_eSocketState = ESocketStatePrefix::disconnected;
		misc::CCounterScoped counter(*this);

		if (!counter.isStartOperation())
		{
			/** ������ ����������� */
			clientDisconnected(std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category()));
			return;
		}

		try
		{
			/** ����������� ����� */
			_socket = socket::CSocketHandle(SOCK_STREAM, IPPROTO_TCP);

			if (!_socket.isValid())
			{
				clientDisconnected(
					std::error_code(WSAGetLastError(), std::system_category()));
				return;
			}

			/** ������� ����������� ����� */
			changeHandle(_socket);		

			/** ��������� ������ �� ������, ������� ������������ ���������� */
			clientDisconnected(_ec);
		}
		catch (const std::exception& ex)
		{
			/** ������ ����������� */
			_pIocp->log(logger::EMessageType::warning, ex);

			clientDisconnected(std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category()));
		}

		break;
	}
	case ESocketStatePrefix::connecting:
		break;
	case ESocketStatePrefix::connected:
	{
		_eSocketState = ESocketStatePrefix::disconnecting;
		_ec = ec;

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

	if (ec || dwReturnSize == 0)
		disconnect(
			ec ? ec : std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));

	clientAsyncSendComplitionHandler(bufferSend, dwReturnSize, ec);

	endOperation();
}
//==============================================================================
void CTcpClientPrefix::asyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	assert(bufferRecv != nullptr);

	if (ec || dwReturnSize == 0)
		disconnect(
			ec ? ec : std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));

	clientAsyncRecvComplitionHandler(bufferRecv, dwReturnSize, ec);

	endOperation();
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
CTcpClientPrefix::~CTcpClient()
{
	/** ��������� */
	disconnect();

	/** ���� ���������� ����� */
	release();
}
//==============================================================================