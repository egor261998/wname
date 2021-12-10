#include "../stdafx.h"

using CTcpClientPrefix = wname::network::CTcpServer;
using ESocketStatePrefix = wname::network::socket::ESocketState;

//==============================================================================
CTcpClientPrefix::CTcpServer(
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
				"Create of ListenSocket socket failed with error: " +
				std::to_string(dwResult);

			throw std::runtime_error(strError);
		}

		/** ����������� ����� � ������ */
		if (bind(_socket,
			_socketAddress,
			_socketAddress.size()) == SOCKET_ERROR)
		{
			const auto dwResult = WSAGetLastError();

			std::string strError = "bind failed with error: " +
				std::to_string(dwResult);

			throw std::runtime_error(strError);
		}

		/** ��������� ����� � ����� ��������� */
		if (listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		{
			const auto dwResult = WSAGetLastError();

			std::string strError = "listen failed with error: " +
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
std::error_code CTcpClientPrefix::connectServer()
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

			try
			{
				ec = startListen();
				_eSocketState = ESocketStatePrefix::connected;
			}
			catch (const std::exception& ex)
			{
				_eSocketState = ESocketStatePrefix::disconnected;
				_pIocp->log(logger::EMessageType::critical, ex);
				throw;
			}
		}
		else
		{
			/** ���-�� ��� ����������� */
			ec = std::error_code(ERROR_INVALID_STATE, std::system_category());
		}
	}

	serverConnected(ec);

	return ec;
}
//==============================================================================
void CTcpClientPrefix::disconnectServer(
	const std::error_code ec) noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	switch (_eSocketState)
	{
	case ESocketStatePrefix::disconnected:
		break;
	case ESocketStatePrefix::disconnecting:
	{
		if (_listClients.empty())
			return;

		_eSocketState = ESocketStatePrefix::disconnected;
		serverDisconnected(_ec);
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
		try
		{
			/** ����������� ���� �������� */
			for (auto it = _listClients.begin(); it != _listClients.end(); )
			{
				auto itCur = it;
				it++;

				itCur->second.release();

				itCur->first->deleteAfterEndOperation();
				itCur->first->disconnect();
			}
		}
		catch (const std::exception& ex)
		{
			_pIocp->log(logger::EMessageType::warning, ex);
		}
		
		disconnectServer();
		break;
	}
	default:
		break;
	}
}
//==============================================================================
void CTcpClientPrefix::removeClient(
	CTcpConnectedClient* const pClient) noexcept
{
	if (pClient == nullptr)
		return;

	cs::CCriticalSectionScoped lock(_csCounter);
	bool bAlreadyDelete = true;

	try
	{	
		if (const auto& it = _listClients.find(pClient); it != _listClients.end())
		{
			bAlreadyDelete = false;
			pClient->deleteAfterEndOperation();
			it->second.release();
			_listClients.erase(it);
		}
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
	}

	/** ���� ������ � ������ ����������, �� ����� �������� ������ */
	if (_eSocketState == ESocketStatePrefix::disconnecting)
		disconnectServer();

	/** ������� ������, ������� ������ ��� � ������ */
	if(!bAlreadyDelete)
		endOperation();
}
//==============================================================================
CTcpClientPrefix::CTcpConnectedClient* CTcpClientPrefix::addClient()
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return nullptr;

	cs::CCriticalSectionScoped lock(_csCounter);

	try
	{
		auto pClient = createClient();
		const auto p = pClient.get();
		_listClients[p] = std::move(pClient);

		counter.release();
		return p;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTcpClientPrefix::clientAcceptedEventHandler(
	io::iocp::CAsyncOperation* pAsyncOperation) noexcept
{
	#pragma warning (disable: 26429)
	/** ����������� ����������� �������� */
	assert(pAsyncOperation != nullptr);

	/** �������� �������� ������� */
	assert(pAsyncOperation->_pCompletionRoutineContext != nullptr);
	auto _this = static_cast<CTcpConnectedClient*>(
		pAsyncOperation->_pCompletionRoutineContext);

	auto pParent = _this->_pParent;
	std::error_code ec;

	/** ������ ��������� */
	_this->_eSocketState = ESocketStatePrefix::connected;

	/** ��������� �� ������ �������� ������� */
	if (pAsyncOperation->_ec.value())
	{
		/** ���������� ������������ ����� */
		pParent->removeClient(_this);
		pParent->disconnectServer(pAsyncOperation->_ec);
		pParent->endOperation();
		return;
	}

	/** ������ �� ����� �������� */
	try
	{
		sockaddr* pLocal = nullptr;
		sockaddr* pRemote = nullptr;
		int localLen = 0;
		int remoteLen = 0;
		_this->_pParent->_socket.getGetAcceptExSockAddrs()(
			pAsyncOperation->_buffer._p,
			0,
			sizeof(_this->_bufferConnect) / 2,
			sizeof(_this->_bufferConnect) / 2,
			&pLocal,
			&localLen,
			&pRemote,
			&remoteLen);

		_this->_localAddres = socket::CSocketAddress(pLocal);
		_this->_remotelAddres = socket::CSocketAddress(pRemote);

		ec = pParent->startListen();
	}
	catch (const std::exception& ex)
	{
		/** �� ���������� */
		_this->_pIocp->log(logger::EMessageType::warning, ex);

		ec = std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());
		pParent->removeClient(_this);
		pParent->disconnectServer(ec);
		pParent->endOperation();
		return;
	}

	/** �������� ���������� ������� */
	pParent->clientConnected(_this, ec);
	if (ec)
	{
		_this->disconnect(ec);
		pParent->disconnectServer(ec);
	}
	pParent->endOperation();
}
//==============================================================================
std::error_code CTcpClientPrefix::startListen()
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		/** �������� ����� ����������� �������� */
		auto pAsyncOperation = _pIocp->getAsyncOperation(
			nullptr, clientAcceptedEventHandler);

		assert(pAsyncOperation != nullptr);
		try
		{
			cs::CCriticalSectionScoped lock(_csCounter);

			auto pClient = addClient();
			if (pClient == nullptr)
			{
				/** ��������� ������ */
				pAsyncOperation->cancel();
				return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
			}

			/** ���������� �������� ������� */
			pAsyncOperation->_pCompletionRoutineContext = pClient;
			pAsyncOperation->_buffer._p = &pClient->_bufferConnect[0];
			pAsyncOperation->_buffer._dwSize = sizeof(pClient->_bufferConnect);

			/** ������� ������ �� �������� */
			if (!_socket.getAcceptEx()(
				_socket,
				pClient->_socket,
				pAsyncOperation->_buffer._p,
				0,
				sizeof(pClient->_bufferConnect) / 2,
				sizeof(pClient->_bufferConnect) / 2,
				&pAsyncOperation->_dwReturnSize,
				&pAsyncOperation->_overlapped))
			{
				const DWORD dwError = WSAGetLastError();

				if (dwError != ERROR_IO_PENDING)
				{
					removeClient(pClient);

					return std::error_code(dwError, std::system_category());
				}
			}

			counter.release();
			return std::error_code();
		}
		catch (const std::exception&)
		{
			pAsyncOperation->cancel();
			throw;
		}
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::unique_ptr<CTcpClientPrefix::CTcpConnectedClient> CTcpClientPrefix::createClient()
{
	try
	{
		/** �������� ������� ��-��������� */
		return std::make_unique<CTcpConnectedClient>(this);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTcpClientPrefix::clientAsyncRecvComplite(
	CTcpConnectedClient* const pTcpClient,
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(pTcpClient);
	UNREFERENCED_PARAMETER(bufferRecv);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpClientPrefix::clientAsyncSendComplite(
	CTcpConnectedClient* const pTcpClient,
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(pTcpClient);
	UNREFERENCED_PARAMETER(bufferSend);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpClientPrefix::clientConnected(
	CTcpConnectedClient* const pTcpClient,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(pTcpClient);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpClientPrefix::clientDisconnected(
	CTcpConnectedClient* const pTcpClient,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(pTcpClient);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpClientPrefix::serverConnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpClientPrefix::serverDisconnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
CTcpClientPrefix::~CTcpServer()
{
	/****��������� */
	disconnectServer();

	/** ���� ���������� ����� */
	release();
}
//==============================================================================