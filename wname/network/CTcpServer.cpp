#include "../stdafx.h"

using wname::network::CTcpServer;
using wname::network::socket::ESocketState;

//==============================================================================
CTcpServer::CTcpServer(
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) :
	CSocketIo(pIocp)
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
				"Create of ListenSocket socket failed with error: " +
				std::to_string(dwResult);

			throw std::runtime_error(strError);
		}

		/** привязываем сокет к адресу */
		if (bind(_socket,
			_socketAddress,
			_socketAddress.size()) == SOCKET_ERROR)
		{
			const auto dwResult = WSAGetLastError();

			std::string strError = "bind failed with error: " +
				std::to_string(dwResult);

			throw std::runtime_error(strError);
		}

		/** переводим сокет в режим слушателя */
		if (listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		{
			const auto dwResult = WSAGetLastError();

			std::string strError = "listen failed with error: " +
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
std::error_code CTcpServer::connectServer()
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	std::error_code ec;

	{
		/** отдельная область видимости для удобства блокировки */
		cs::CCriticalSectionScoped lock(_csCounter);

		if (_eSocketState == ESocketState::disconnected)
		{
			/** действуем */
			_eSocketState = ESocketState::connecting;

			try
			{
				ec = startListen();
				_eSocketState = ESocketState::connected;
			}
			catch (const std::exception& ex)
			{
				_eSocketState = ESocketState::disconnected;
				_pIocp->log(logger::EMessageType::critical, ex);
				throw;
			}
		}
		else
		{
			/** кто-то уже соединяется */
			ec = std::error_code(ERROR_INVALID_STATE, std::system_category());
		}
	}

	serverConnected(ec);

	return ec;
}
//==============================================================================
void CTcpServer::disconnectServer(
	const std::error_code ec) noexcept
{
	/** счетчик операций для корректного завершения контекста */
	misc::CCounterScoped counter(*this);

	bool bIsRepeatDisconnect = false;
	bool bIsDisconnected = false;
	std::unordered_map<CTcpConnectedClient*, std::shared_ptr<CTcpConnectedClient>> listClients;
	std::error_code ecDisconnected;

	{
		cs::CCriticalSectionScoped lock(_csCounter);

		switch (_eSocketState)
		{
		case ESocketState::disconnected:
			break;
		case ESocketState::disconnecting:
		{
			if (!_listClients.empty())
			{
				listClients = std::move(_listClients);
				bIsRepeatDisconnect = true;
				break;
			}

			_eSocketState = ESocketState::disconnected;
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

			shutdown(_socket, SD_BOTH);
			CancelIoEx(_socket, nullptr);
			listClients = std::move(_listClients);
			bIsRepeatDisconnect = true;
			break;
		}
		default:
			break;
		}
	}

	if (bIsRepeatDisconnect)
	{
		try
		{
			/** сворачиваем всех клиентов */
			listClients.clear();
		}
		catch (const std::exception& ex)
		{
			_pIocp->log(logger::EMessageType::warning, ex);
		}

		/** повторное отключение */
		disconnectServer();
	}

	if (bIsDisconnected)
	{
		/** отключаем сервер*/
		serverDisconnected(ecDisconnected);
	}
}
//==============================================================================
void CTcpServer::removeClient(
	CTcpConnectedClient* const pClient) noexcept
{
	if (pClient == nullptr)
		return;

	bool bIsRepeatDisconnect = true;
	std::shared_ptr<CTcpConnectedClient> pClientPtr;

	{
		cs::CCriticalSectionScoped lock(_csCounter);		

		try
		{
			if (const auto& it = _listClients.find(pClient); it != _listClients.end())
			{
				pClientPtr = std::move(it->second);
				_listClients.erase(it);
			}
		}
		catch (const std::exception& ex)
		{
			_pIocp->log(logger::EMessageType::critical, ex);
		}

		/** если сервер в стадии отключения, то нужно изменить статус */
		bIsRepeatDisconnect = _eSocketState == ESocketState::disconnecting;
	}

	/** сброс клиента */
	pClientPtr.reset();

	if (bIsRepeatDisconnect)
	{
		/** повторное отключение */
		disconnectServer();
	}
}
//==============================================================================
CTcpServer::CTcpConnectedClient* CTcpServer::addClient()
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return nullptr;

	cs::CCriticalSectionScoped lock(_csCounter);
	
	if (_eSocketState != ESocketState::connected &&
		_eSocketState != ESocketState::connecting)
	{
		/** сервер не включен , сваливаем */
		return nullptr;
	}

	try
	{
		auto pClient = std::shared_ptr<CTcpConnectedClient>(
			createClient().release(),
			[](CTcpConnectedClient* const pClient) noexcept
			{
				if (pClient == nullptr)
					return;

				pClient->disconnect();
				pClient->deleteAfterEndOperation();
			});
		const auto p = pClient.get();
		_listClients[p] = std::move(pClient);

		return p;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTcpServer::clientAcceptedEventHandler(
	io::iocp::CAsyncOperation* pAsyncOperation) noexcept
{
	#pragma warning (disable: 26429)
	/** отсутствует асинхронная операция */
	assert(pAsyncOperation != nullptr);

	/** получаем контекст клиента */
	assert(pAsyncOperation->_pCompletionRoutineContext != nullptr);
	auto _this = static_cast<CTcpConnectedClient*>(
		pAsyncOperation->_pCompletionRoutineContext);

	auto pParent = _this->_pParent;
	std::error_code ec;

	/** проверяем на ошибку ожидания клиента */
	if (pAsyncOperation->_ec.value())
	{
		/** приложение инициировало выход */
		pParent->removeClient(_this);
		_this->endOperation();

		pParent->disconnectServer(pAsyncOperation->_ec);
		pParent->endOperation();
		return;
	}

	/** встаем на новое ожидание */
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

		_this->_localAddress = socket::CSocketAddress(pLocal);
		_this->_remotelAddress = socket::CSocketAddress(pRemote);
	
		/** новый клиент в очередь */
		ec = pParent->startListen();		
	}
	catch (const std::exception& ex)
	{
		/** не получилось */
		_this->_pIocp->log(logger::EMessageType::warning, ex);

		ec = std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
		pParent->removeClient(_this);
		_this->endOperation();

		pParent->disconnectServer(ec);
		pParent->endOperation();
		return;
	}

	/** клиент подключен */
	_this->_eSocketState = ESocketState::connected;

	/** передаем управление клиенту */
	_this->clientConnected(ec);
	if (ec)
	{
		_this->disconnect(ec);
		pParent->disconnectServer(ec);
	}

	_this->endOperation();
	pParent->endOperation();
}
//==============================================================================
std::error_code CTcpServer::startListen()
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		/** выделяем новую асинхронную операцию */
		auto pAsyncOperation = _pIocp->getAsyncOperation(
			nullptr, clientAcceptedEventHandler);

		assert(pAsyncOperation != nullptr);
		try
		{
			cs::CCriticalSectionScoped lock(_csCounter);

			auto pClient = addClient();
			if (pClient == nullptr)
			{
				/** отвалился сервер */
				pAsyncOperation->cancel();
				return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
			}

			/** выставляем контекст клиента */
			pAsyncOperation->_pCompletionRoutineContext = pClient;
			pAsyncOperation->_buffer._p = &pClient->_bufferConnect[0];
			pAsyncOperation->_buffer._dwSize = sizeof(pClient->_bufferConnect);
		
			try
			{
				/** ссылка на операцию подключения клиента */
				if (!pClient->startOperation())
				{
					removeClient(pClient);
					pAsyncOperation->cancel();
					return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
				}

				/** пробуем встать на ожидание */
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
						pAsyncOperation->cancel();
						return std::error_code(dwError, std::system_category());
					}
				}
			}
			catch (const std::exception&)
			{
				removeClient(pClient);
				throw;
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
std::unique_ptr<CTcpServer::CTcpConnectedClient> CTcpServer::createClient()
{
	try
	{
		/** создание клиента по-умолчанию */
		return std::make_unique<CTcpConnectedClient>(this);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTcpServer::serverConnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpServer::serverDisconnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpServer::release(
	const bool bIsWait) noexcept
{
	__super::release(false);
	/** отключаем */
	disconnectServer();

	/** ждем завершения всего */
	if(bIsWait)
	{ 
		__super::release(bIsWait);
	}
}
//==============================================================================
CTcpServer::~CTcpServer()
{
	/** завершение всего */
	release(true);
}
//==============================================================================