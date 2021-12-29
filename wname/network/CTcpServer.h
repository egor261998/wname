#pragma once

_WNAME_BEGIN
namespace network
{
	/** класс TCP сервера */
	class CTcpServer : protected socket::CSocketIo
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** класс подключенного клиента к TCP серверу */
		class CTcpConnectedClient;
		friend class CTcpConnectedClient;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор TCP сервера.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		WNAME CTcpServer(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* включить TCP сервер.
		* @return - результат выполнения.
		*/
		WNAME std::error_code connectServer();
	//==========================================================================
		/**
		* отключить TCP сервер.
		* @param ec - код ошибки завершения.
		*/
		WNAME void disconnectServer(
			const std::error_code ec = std::error_code()) noexcept;
	//==========================================================================
		/**
		* деструктор TCP сервера.
		*/
		WNAME ~CTcpServer();
	//==========================================================================
		CTcpServer(const CTcpServer&) = delete;
		CTcpServer(CTcpServer&&) = delete;
		CTcpServer& operator=(const CTcpServer&) = delete;
		CTcpServer& operator=(CTcpServer&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* удаление клиента из списка.
		* @param pClient - удаляемый клиент.
		*/
		WNAME void removeClient(
			CTcpConnectedClient* const pClient) noexcept;
	//==========================================================================
		/**
		* добавить клиента в список.
		* @return - подключаемый клиент.
		*/
		WNAME CTcpConnectedClient* addClient();
	//==========================================================================
		/**
		* обработчик события принятия нового подключения со всех TCP серверов.
		* @param pAsyncOperation - асинхронная операция.
		*/
		WNAME static void clientAcceptedEventHandler(
			io::iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* старт прослушивания для ожидания нового подключения.
		* @return - код ошибки.
		*/
		WNAME std::error_code startListen();
	//==========================================================================
		/**
		* виртуальный обработчик создания клиента.
		* @return - клиент
		*/
		WNAME virtual std::unique_ptr<CTcpConnectedClient> createClient();
	//==========================================================================
		/**
		* виртуальный обработчик события отключения сервера.
		* @param ec - код ошибки.
		*/
		WNAME virtual void serverConnected(
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события отключения сервера.
		* @param ec - код ошибки.
		*/
		WNAME virtual void serverDisconnected(
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** состояние */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** адрес сервера */
		socket::CSocketAddress _socketAddress;
		/** сокет клиента */
		socket::CSocketHandle _socket;

		/** подключенные клиенты */
		std::unordered_map<CTcpConnectedClient*, std::shared_ptr<CTcpConnectedClient>> _listClients;
	
		/** ошибка */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};

	using CTcpConnectedClient = CTcpServer::CTcpConnectedClient;
}
_WNAME_END