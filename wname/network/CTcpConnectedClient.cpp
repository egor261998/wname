#include "../stdafx.h"

using wname::network::CTcpServer;
using wname::network::socket::CSocketAddress;
using wname::network::socket::ESocketState;

//==============================================================================
CTcpServer::CTcpConnectedClient::CTcpConnectedClient(
	CTcpServer* pParent) :
	ITcpClient(pParent->_pIocp),
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
std::error_code  CTcpServer::CTcpConnectedClient::connect() noexcept
{
	return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
}

//==============================================================================
CSocketAddress CTcpServer::CTcpConnectedClient::getAddress() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _localAddress;
}
//==============================================================================
CSocketAddress CTcpServer::CTcpConnectedClient::getAddressRemote() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _remotelAddress;
}
//==============================================================================
void CTcpServer::CTcpConnectedClient::disconnect(
	const std::error_code ec) noexcept
{
	/** счетчик операций для корректного завершения контекста */
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
			/** еще не все операции отработали */
			if (_nAsyncIoPending > 0)
				break;

			_eSocketState = ESocketState::disconnected;
			_socket.close();

			/** просто закрываемся */
			ecDisconnected = _ec;
			bIsDisconnected = true;

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
void CTcpServer::CTcpConnectedClient::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);	
}
//==============================================================================
CTcpServer::CTcpConnectedClient::~CTcpConnectedClient()
{
	/** завершение всего */
	release(true);

	/** снимаем ссылку с сервера */
	_pParent->endOperation();
}
//==============================================================================