#pragma once

_WNAME_BEGIN
namespace network
{
	/** класс TCP клиента */
	class CTcpClient : protected socket::CSocketIo
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор TCP клиента.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		WNAME CTcpClient(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* синхронное соединение.
		* @return - код ошибки.
		*/
		WNAME std::error_code connect() noexcept;
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
		* получить состояние сокета.
		* @return - состояние сокета.
		*/
		WNAME socket::ESocketState getState() noexcept;
	//==========================================================================
		/**
		* разорвать соединение.
		* @param ec - код ошибки завершения.
		*/
		WNAME void disconnect(
			const std::error_code ec = std::error_code()) noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CTcpClient();
	//==========================================================================
		CTcpClient(const CTcpClient&) = delete;
		CTcpClient(CTcpClient&&) = delete;
		CTcpClient& operator=(const CTcpClient&) = delete;
		CTcpClient& operator=(CTcpClient&&) = delete;
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

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** состояние */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** адрес сервера */
		socket::CSocketAddress _socketAddress;
		/** сокет клиента */
		socket::CSocketHandle _socket;

		/** ошибка */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END