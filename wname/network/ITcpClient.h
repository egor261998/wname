#pragma once

_WNAME_BEGIN
namespace network
{
	/** интерфейс TCP клиента */
	class WNAME ITcpClient : protected socket::CSocketIo
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор TCP интерфейса.
		* @param pIocp - механизм Iocp.
		*/
		ITcpClient(
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* виртуальное синхронное соединение.
		* @return - код ошибки.
		*/
		virtual std::error_code connect() noexcept = 0;
	//==========================================================================
		/**
		* установить значение keep alive для клиента.
		* @param bValue - значение установки.
		* @return - код ошибки.
		*/
		std::error_code setKeepAlive(
			const bool bValue) noexcept;
	//==========================================================================
		/**
		* старт асинхронной записи в сокет.
		* @param bufferSend - буфер для записи.
		* @param dwBufferSize - размер буфера для записи.
		* @param dwFlags - флаги.
		* @return - код ошибки.
		*/
		std::error_code startAsyncSend(
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
		std::error_code startSend(
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
		std::error_code startAsyncRecv(
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
		std::error_code startRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* получить адрес сокета клиента.
		* @return - адрес сокета.
		*/
		virtual socket::CSocketAddress getAddress() noexcept = 0;
	//==========================================================================
		/**
		* получить адрес сокета удаленного клиента.
		* @return - адрес сокета.
		*/
		virtual socket::CSocketAddress getAddressRemote() noexcept = 0;
	//==========================================================================
		/**
		* получить состояние сокета.
		* @return - состояние сокета.
		*/
		socket::ESocketState getState() noexcept;
	//==========================================================================
		/**
		* виртуально разорвать соединение.
		* @param ec - код ошибки завершения.
		*/
		virtual void disconnect(
			const std::error_code ec = std::error_code()) noexcept = 0;
	//==========================================================================
		/**
		* закончить работу.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		~ITcpClient();
	//==========================================================================
		ITcpClient(const ITcpClient&) = delete;
		ITcpClient(ITcpClient&&) = delete;
		ITcpClient& operator=(const ITcpClient&) = delete;
		ITcpClient& operator=(ITcpClient&&) = delete;
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
		virtual void clientAsyncRecvComplitionHandler(
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
		virtual void clientAsyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события подключения клиента.
		* @param ec - код ошибки.
		*/
		virtual void clientConnected(
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события отключения клиента.
		* @param ec - код ошибки.
		*/
		virtual void clientDisconnected(
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* добавить асинхронную операцию в обработку.
		*/
		void addAsyncIoPending() noexcept;
	//==========================================================================
		/**
		* удалить асинхронную операцию из обработки.
		*/
		void delAsyncIoPending() noexcept;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи.
		* @param bufferSend - буфер данных.
		* @param dwReturnSize - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		void asyncSendComplitionHandler(
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
		void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** состояние */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** сокет клиента */
		socket::CSocketHandle _socket;

		/** количество асинхронных операций в обработке */
		size_t _nAsyncIoPending = 0;

		/** ошибка */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END