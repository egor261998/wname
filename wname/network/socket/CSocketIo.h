#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** операции ввода/вывода с сокетом */
	class WNAME CSocketIo : protected io::CAsyncIo
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		* @param pIocp - механизм ввода/вывода.
		*/
		CSocketIo(
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* старт асинхронного чтения.
		* @param bufferRecv - буфер для приема.
		* @param bufferSize - размер буфера для чтения.
		* @param dwFlags - флаги.
		* @return - код ошибки.
		*/
		virtual std::error_code startAsyncRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const DWORD dwFlags = 0u);
	//==========================================================================
		/**
		* старт чтения.
		* @param bufferRecv - буфер для приема.
		* @param bufferSize - размер буфера для чтения.
		* @param pdwReturnSize - количество прочитанных байт.
		* @param dwFlags - флаги.
		* @return - код ошибки.
		*/
		virtual std::error_code startRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u);
	//==========================================================================
		/**
		* старт асинхронной записи.
		* @param bufferSend - буфер для отправки.
		* @param bufferSize - размер буфера для записи.
		* @param dwFlags - флаги.
		* @return - код ошибки.
		*/
		virtual std::error_code startAsyncSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize,
			const DWORD dwFlags = 0u);
	//==========================================================================
		/**
		* старт записи.
		* @param bufferSend - буфер для отправки.
		* @param bufferSize - размер буфера для записи.
		* @param pdwReturnSize - количество записанных байт.
		* @param dwFlags - флаги.
		* @return - код ошибки.
		*/
		virtual std::error_code startSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u);
	//==========================================================================
		/**
		* закончить работу и дождаться всех асинхронных операций.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		~CSocketIo();
	//==========================================================================
		CSocketIo(const CSocketIo&) = delete;
		CSocketIo(CSocketIo&&) = delete;
		CSocketIo& operator=(const CSocketIo&) = delete;
		CSocketIo& operator=(CSocketIo&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронного чтения.
		* @param bufferRecv - буфер данных.
		* @param dwReturnSize - количество прочитанных байт.
		* @param ec - код ошибки завершения.
		*/
		virtual void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронной записи.
		* @param bufferSend - буфер данных.
		* @param dwReturnSize - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		virtual void asyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* обработчик события завершения асинхронного чтения.
		* @param bufferRead - буфер данных.
		* @param dwReturnSize - количество прочитанных байт.
		* @param ec - код ошибки завершения.
		*/
		void asyncReadComplitionHandler(
			const PBYTE bufferRead,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи.
		* @param bufferWrite - буфер данных.
		* @param dwReturnSize - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		void asyncWriteComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END