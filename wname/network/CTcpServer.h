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
		void removeClient(
			CTcpConnectedClient* const pClient) noexcept;
	//==========================================================================
		/**
		* добавить клиента в список.
		* @return - подключаемый клиент.
		*/
		CTcpConnectedClient* addClient();
	//==========================================================================
		/**
		* обработчик события принятия нового подключения со всех TCP серверов.
		* @param pAsyncOperation - асинхронная операция.
		*/
		static void clientAcceptedEventHandler(
			io::iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* старт прослушивания для ожидания нового подключения.
		* @return - код ошибки.
		*/
		std::error_code startListen();
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронного чтения.
		* @param pTcpClient - клиент.
		* @param bufferRecv - буфер данных.
		* @param dwBufferSize - количество прочитанных байт.
		* @param ec - код завершения.
		*/
		virtual void clientAsyncRecvComplite(
			CTcpConnectedClient* const pTcpClient,
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронной записи.
		* @param pTcpClient - клиент.
		* @param bufferSend - количество отправленных байт.
		* @param dwBufferSize - количество записанных байт.
		* @param ec - код завершения.
		*/
		virtual void clientAsyncSendComplite(
			CTcpConnectedClient* const pTcpClient,
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* виртуальный обработчик события подключения нового клиента.
		* @param pTcpClient - ссылка на текущего клиента.
		* @param ec - код ошибки завершения.
		*/
		virtual void clientConnected(
			CTcpConnectedClient* const pTcpClient,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* виртуальный обработчик события отключения клиента.
		* @param pTcpClient - клиент.
		* @param ec - код ошибки завершения.
		*/
		virtual void clientDisconnected(
			CTcpConnectedClient* const pTcpClient,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* виртуальный обработчик события отключения сервера.
		* @param ec - код ошибки.
		*/
		virtual void serverConnected(
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* виртуальный обработчик события отключения сервера.
		* @param ec - код ошибки.
		*/
		virtual void serverDisconnected(
			const std::error_code ec) noexcept = 0;
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
		std::unordered_map<CTcpConnectedClient*, std::unique_ptr<CTcpConnectedClient>> _listClients;
	
		/** ошибка */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};

	using CTcpConnectedClient = CTcpServer::CTcpConnectedClient;
}
_WNAME_END