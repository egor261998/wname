#include "../stdafx.h"

using wname::network::CTcpClient;
using wname::network::socket::CSocketAddress;
using wname::network::socket::ESocketState;

//==============================================================================
CTcpClient::CTcpClient(
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) :
	ITcpClient(pIocp)
{
	try
	{
		_socketAddress = socket::CSocketAddress(strIp, wPort);
	}
	catch (const std::exception& ex)
	{
		pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTcpClient::connect() noexcept
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	std::error_code ec;

	{
		/** ��������� ������� ��������� ��� �������� ���������� */
		cs::CCriticalSectionScoped lock(_csCounter);

		if (_eSocketState == ESocketState::disconnected)
		{
			/** ��������� */
			_eSocketState = ESocketState::connecting;

			/** ������� ����������� */
			if (::connect(_socket, _socketAddress, _socketAddress.size()) ==
				SOCKET_ERROR)
			{
				ec = std::error_code(WSAGetLastError(), std::system_category());
				_eSocketState = ESocketState::disconnected;
			}
			else
			{
				_eSocketState = ESocketState::connected;
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
void CTcpClient::disconnect(
	const std::error_code ec) noexcept
{
	/** ������� �������� ��� ����������� ���������� ��������� */
	misc::CCounterScoped counter(*this);

	bool bIsRepeatDisconnect = false;
	bool bIsDisconnected = false;
	std::error_code ecDisconnected;

	{
		cs::CCriticalSectionScoped lock(_csCounter);

		switch (_eSocketState)
		{
		case ESocketState::disconnected:
			break;
		case ESocketState::disconnecting:
		{
			/** ��� �� ��� �������� ���������� */
			if (_nAsyncIoPending > 0)
				break;

			_eSocketState = ESocketState::disconnected;
			if (!counter.isStartOperation())
			{
				/** ������ ����������� */
				ecDisconnected = std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
				bIsDisconnected = true;
				break;
			}

			try
			{
				/** ����������� ����� */
				_socket = socket::CSocketHandle(SOCK_STREAM, IPPROTO_TCP);

				if (!_socket.isValid())
				{
					ecDisconnected = std::error_code(WSAGetLastError(), std::system_category());
					bIsDisconnected = true;
					break;
				}

				/** ������� ����������� ����� */
				changeHandle(_socket);

				/** ��������� ������ �� ������, ������� ������������ ���������� */
				ecDisconnected = _ec;
				bIsDisconnected = true;
			}
			catch (const std::exception& ex)
			{
				/** ������ ����������� */
				_pIocp->log(logger::EMessageType::warning, ex);

				ecDisconnected = std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
				bIsDisconnected = true;
			}

			break;
		}
		case ESocketState::connecting:
			break;
		case ESocketState::connected:
		{
			_eSocketState = ESocketState::disconnecting;
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
		/** ��������� ���������� */
		disconnect();
	}

	if (bIsDisconnected)
	{
		/** ��������� ������� */
		clientDisconnected(ecDisconnected);
	}
}
//==============================================================================
CSocketAddress CTcpClient::getAddress() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _socketAddress;
}
//==============================================================================
CSocketAddress CTcpClient::getAddressRemote() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _socketAddress;
}
//==============================================================================
void CTcpClient::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//==============================================================================
CTcpClient::~CTcpClient()
{
	/** ���������� ����� */
	release(true);
}
//==============================================================================