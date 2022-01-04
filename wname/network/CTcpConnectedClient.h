#pragma once

_WNAME_BEGIN
namespace network
{
	/** класс подключенного клиента к TCP серверу */
	class CTcpServer::CTcpConnectedClient : protected CSocketIo
	{
		friend class CTcpServer;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор подключаемого клиента.
		* @param pParent - родительный класс TCP сервера.
		*/
		WNAME CTcpConnectedClient(
			CTcpServer* const pParent);
	//==========================================================================
			/**
			* установить значение keep alive для клиента.
			* @param bValue - значение установки.
			* @return - код ошибки.
			*/
		WNAME std::error_code setKeepAlive(
			const bool bValue) noexcept;
	//==========================================================================
		/**
		* получить адрес сокета клиента.
		* @return - адрес сокета клиента.
		*/
		WNAME socket::CSocketAddress getClientAddress() noexcept;
		//==========================================================================
		/**
		* получить адрес сокета сервера.
		* @return - адрес сокета сервера.
		*/
		WNAME socket::CSocketAddress getServerAddress() noexcept;
	//==========================================================================
		/**
		* получить состояние сокета.
		* @return - состояние сокета.
		*/
		WNAME socket::ESocketState getState() noexcept;
	//==========================================================================
		/**
		* старт асинхронной записи в сокет.
		* @param bufferSend - буфер для записи.
		* @param dwBufferSize - размер буфера для записи.
		* @param dwFlags - флаги.
		* @return - код ошибки.
		*/
		WNAME std::error_code startAsyncSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* старт синхронной записи в сокет.
		* @param bufferSend - буфер для записи.
		* @param dwBufferSize - размер буфера для записи.
		* @param pdwReturnSize - количество записанных байт.
		* @param dwFlags - флаги.
		* @return - код ошибки.
		*/
		WNAME std::error_code startSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* старт асинхронной чтения из сокета.
		* @param bufferRecv - буфер для записи.
		* @param dwBufferSize - размер буфера для записи.
		* @param dwFlags - флаги.
		* @return - код ошибки.
		*/
		WNAME std::error_code startAsyncRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* старт синхронного чтения из сокета.
		* @param bufferRecv - буфер для записи.
		* @param dwBufferSize - размер буфера для записи.
		* @param pdwReturnSize - количество записанных байт.
		* @param dwFlags - флаги.
		* @return - код ошибки.
		*/
		WNAME std::error_code startRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* разорвать соединение.
		* @param ec - код ошибки.
		*/
		WNAME void disconnect(
			const std::error_code ec = std::error_code()) noexcept;
	//==========================================================================
		/**
		* закончить работу.
		*/
		WNAME void release() noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CTcpConnectedClient();
	//==========================================================================
		CTcpConnectedClient(const CTcpConnectedClient&) = delete;
		CTcpConnectedClient(CTcpConnectedClient&&) = delete;
		CTcpConnectedClient& operator=(const CTcpConnectedClient&) = delete;
		CTcpConnectedClient& operator=(CTcpConnectedClient&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронного чтения.
		* @param bufferRecv - буфер данных.
		* @param dwReturnSize - количество полученных байт.
		* @param ec - код завершения.
		*/
		WNAME virtual void clientAsyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронной записи.
		* @param bufferSend - буфер данных.
		* @param dwReturnSize - количество переданных байт.
		* @param ec - код завершения.
		*/
		WNAME virtual void clientAsyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события подключения клиента.
		* @param ec - код ошибки.
		*/
		WNAME virtual void clientConnected(
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события отключения клиента.
		* @param ec - код ошибки.
		*/
		WNAME virtual void clientDisconnected(
			const std::error_code ec) noexcept;	
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* добавить асинхронную операцию в обработку.
		*/
		WNAME void addAsyncIoPending() noexcept;
	//==========================================================================
		/**
		* удалить асинхронную операцию из обработки.
		*/
		WNAME void delAsyncIoPending() noexcept;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи.
		* @param bufferSend - буфер данных.
		* @param dwReturnSize - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME void asyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события завершения асинхронного чтения.
		* @param bufferRecv - буфер данных.
		* @param dwReturnSize - количество прочитанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** указатель на родительный класс */
		CTcpServer* const _pParent;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** состояние */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** локальный адрес */
		socket::CSocketAddress _localAddress;
		/** удаленный адрес */
		socket::CSocketAddress _remotelAddress;
		/** сокет клиента */
		socket::CSocketHandle _socket;

		/** количество асинхронных операций в обработке */
		size_t _nAsyncIoPending = 0;

		/** ошибка */
		std::error_code _ec;

		/** буфер для подключения */
		BYTE _bufferConnect[(sizeof(sockaddr) + 16) * 2]{ 0 };
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END