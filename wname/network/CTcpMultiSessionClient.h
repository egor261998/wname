#pragma once

_WNAME_BEGIN
namespace network
{
	/** клиент много сессионного сетевого обмена */
	class CTcpMiltiSessionClient : protected ITcpMultiSessionProtocol
	{
	#pragma region Private_Inner
	private:
	//==========================================================================
		class CTcpClientPart : public wname::network::CTcpClient
		{
			friend class CTcpMiltiSessionClient;
		#pragma region Public_Method
		public:
		//======================================================================
			/**
			* конструктор TCP сессии.
			* @param strIp - IP адрес сервера.
			* @param wPort - порт сервера.
			* @param pIocp - механизм Iocp.
			* @param pCTcpMiltiSessionClient - родительный класс.
			*/
			WNAME CTcpClientPart(
				const std::string& strIp,
				const WORD wPort,
				const std::shared_ptr<io::iocp::CIocp>& pIocp,
				CTcpMiltiSessionClient* const pCTcpMiltiSessionClient)
				: CTcpClient(strIp, wPort, pIocp),
				_pCTcpMiltiSessionClient(pCTcpMiltiSessionClient)
			{
				try
				{
					if (_pCTcpMiltiSessionClient == nullptr)
						throw std::invalid_argument("_pCTcpMiltiSessionClient == nullptr");

					if(CoCreateGuid(&_guid) != S_OK)
						throw std::runtime_error("CoCreateGuid is failed");

					/** добавляем ссылку на родительный объект */
					if(!_pCTcpMiltiSessionClient->startOperation())
						throw std::runtime_error("_pCTcpMiltiSessionClient is not start");
				}
				catch (const std::exception& ex)
				{
					pIocp->log(logger::EMessageType::critical, ex);
					throw;
				}
			}
		//======================================================================
			/**
			* обработчик события завершения асинхронного чтения.
			* @param bufferRecv - буфер данных.
			* @param dwReturnSize - количество полученных байт.
			* @param ec - код завершения.
			*/
			WNAME void clientAsyncRecvComplitionHandler(
				const PBYTE bufferRecv,
				const DWORD dwReturnSize,
				const std::error_code ec) noexcept override
			{
				_pCTcpMiltiSessionClient->clientAsyncRecvComplitionHandler(
					this, bufferRecv, dwReturnSize, ec);
			}
		//======================================================================
			/**
			* обработчик события завершения асинхронной записи.
			* @param bufferSend - буфер данных.
			* @param dwReturnSize - количество переданных байт.
			* @param ec - код завершения.
			*/
			WNAME void clientAsyncSendComplitionHandler(
				const PBYTE bufferSend,
				const DWORD dwReturnSize,
				const std::error_code ec) noexcept override
			{
				_pCTcpMiltiSessionClient->clientAsyncSendComplitionHandler(
					this, bufferSend, dwReturnSize, ec);
			}
		//======================================================================
			/**
			* обработчик события подключения клиента.
			* @param ec - код ошибки.
			*/
			WNAME void clientConnected(
				const std::error_code ec) noexcept override
			{
				_pCTcpMiltiSessionClient->clientConnected(
					this, ec);
			}
		//======================================================================
			/**
			* обработчик события отключения клиента.
			* @param ec - код ошибки.
			*/
			WNAME void clientDisconnected(
				const std::error_code ec) noexcept override
			{
				_pCTcpMiltiSessionClient->clientDisconnected(
					this, ec);
			}
		//======================================================================
			/**
			* деструктор.
			*/
			WNAME ~CTcpClientPart()
			{
				/** отключаем */
				disconnect();

				/** ждем завершения всего */
				release();

				/** убираем ссылку с родительного объекта */
				_pCTcpMiltiSessionClient->endOperation();
			}
		//======================================================================
		#pragma endregion

		#pragma region Public_Data
		public:
		//======================================================================
			/** индификатор */
			GUID _guid;
		//======================================================================
		#pragma endregion

		#pragma region Private_Data
		private:
		//======================================================================
			/** родительный класс */
			CTcpMiltiSessionClient* const _pCTcpMiltiSessionClient;
		//======================================================================
		#pragma endregion
		};
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор много сессионного сетевого обмена.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		WNAME CTcpMiltiSessionClient(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* синхронное соединение.
		* @return - код ошибки.
		*/
		WNAME std::error_code connect() noexcept override;
	//==========================================================================
		/**
		* старт асинхронной записи.
		* @param bufferSend - буфер для записи.
		* @param dwBufferSize - размер буфера для записи.
		* @return - код ошибки.
		*/
		WNAME std::error_code startAsyncSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize) override;
	//==========================================================================
		/**
		* старт асинхронного чтения.
		* @param bufferRecv - буфер для записи.
		* @param dwBufferSize - размер буфера для записи.
		* @return - код ошибки.
		*/
		WNAME std::error_code startAsyncRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize) override;
	//==========================================================================
		/**
		* разорвать соединение.
		* @param ec - код ошибки завершения.
		*/
		WNAME void disconnect(
			const std::error_code ec = std::error_code()) noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CTcpMiltiSessionClient();
	//==========================================================================
		CTcpMiltiSessionClient(const CTcpMiltiSessionClient&) = delete;
		CTcpMiltiSessionClient(CTcpMiltiSessionClient&&) = delete;
		CTcpMiltiSessionClient& operator=(const CTcpMiltiSessionClient&) = delete;
		CTcpMiltiSessionClient& operator=(CTcpMiltiSessionClient&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
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
		/**
		* обработчик события подключения клиента.
		* @param pTcpClientPart - клиент.
		* @param ec - код ошибки.
		*/
		WNAME void clientConnected(
			CTcpClientPart* const pTcpClientPart,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* обработчик события завершения асинхронного чтения сессии клиента.
		* @param pTcpClientPart - клиент.
		* @param bufferRecv - буфер данных.
		* @param dwReturnSize - количество полученных байт.
		* @param ec - код завершения.
		*/
		WNAME void clientAsyncRecvComplitionHandler(
			CTcpClientPart* const pTcpClientPart,
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи сессии клиента.
		* @param pTcpClientPart - клиент.
		* @param bufferSend - буфер данных.
		* @param dwReturnSize - количество переданных байт.
		* @param ec - код завершения.
		*/
		WNAME void clientAsyncSendComplitionHandler(
			CTcpClientPart* const pTcpClientPart,
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* обработчик события отключения сессии клиента.
		* @param pTcpClientPart - клиент.
		* @param ec - код ошибки.
		*/
		WNAME void clientDisconnected(
			CTcpClientPart* const pTcpClientPart,
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** управляющая сессия */
		std::unique_ptr<CTcpClientPart> _pTcpClientPartControl;

		/** сессии для передачи */
		std::unordered_map<GUID, std::unique_ptr<CTcpClientPart>> _transferSession;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END