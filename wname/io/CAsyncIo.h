#pragma once

_WNAME_BEGIN
namespace io
{
	/** размеры буферов в КБ */
	#define BUFFER_1K 1024
	#define BUFFER_2K 2048
	#define BUFFER_4K 4096
	#define BUFFER_8K 8192
	#define BUFFER_16K 16384
	#define BUFFER_32K 32768
	#define BUFFER_64K 65536

	/** асинхронный ввод/вывод */
	class CAsyncIo : protected misc::CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		* @param pIocp - механизм ввода/вывода.
		*/
		WNAME CAsyncIo(
			const std::shared_ptr<iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* инициализация асинхронной операции ввода/вывода.
		* @param hHandle - описатель.
		*/
		WNAME void bindHandle(
			handle::CHandle hHandle);
	//==========================================================================
		/**
		* проверка наличия привязанного описателя.
		* @return - результат проверки.
		*/
		WNAME bool isBindHandle() noexcept;
	//==========================================================================
		/**
		* изменить описатель.
		* @param hHandle - описатель.
		*/
		WNAME void changeHandle(
			handle::CHandle hHandle);
	//==========================================================================
		/**
		* закрыть описатель.
		*/
		WNAME void closeHandle() noexcept;
	//==========================================================================
		/**
		* старт асинхронного чтения.
		* @param bufferRead - буфер для чтения
		* @param bufferSize - размер буфера для чтения.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		WNAME std::error_code startAsyncRead(
			const PBYTE bufferRead,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* старт чтения.
		* @param bufferRead - буфер для чтения
		* @param bufferSize - размер буфера для чтения.
		* @param pdwReturnSize - количество прочитанных байт.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		WNAME std::error_code startRead(
			const PBYTE bufferRead,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* старт асинхронной записи.
		* @param bufferWrite - буфер для записи
		* @param bufferSize - размер буфера для записи.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		WNAME std::error_code startAsyncWrite(
			const PBYTE bufferWrite,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* старт записи.
		* @param bufferWrite - буфер для записи
		* @param bufferSize - размер буфера для записи.
		* @param pdwReturnSize - количество записанных байт.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		WNAME std::error_code startWrite(
			const PBYTE bufferWrite,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* получить описатель.
		* @return - описатель.
		*/
		WNAME HANDLE getHandle() noexcept;
	//==========================================================================
		/**
		* закончить работу и дождаться всех асинхронных операций.
		*/
		WNAME void release() noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CAsyncIo();
	//==========================================================================
		CAsyncIo(const CAsyncIo&) = delete;
		CAsyncIo(CAsyncIo&&) = delete;
		CAsyncIo& operator=(const CAsyncIo&) = delete;
		CAsyncIo& operator=(CAsyncIo&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронного чтения.
		* @param bufferRead - буфер данных.
		* @param dwReturnSize - количество прочитанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME virtual void asyncReadComplitionHandler(
			const PBYTE bufferRead,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронной записи.
		* @param bufferWrite - буфер данных.
		* @param dwReturnSize - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME virtual void asyncWriteComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* обработчик события завершения асинхронного чтения.
		* @param pAsyncOperation - асинхронная операция.
		*/
		WNAME static void asyncReadIocpHandler(
			iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи.
		* @param pAsyncOperation - асинхронная операция.
		*/
		WNAME static void asyncWriteIocpHandler(
			iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной операции.
		* @param pAsyncOperation - асинхронная операция.
		*/
		WNAME static void asyncIocpHandler(
			iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** статистика */
		std::atomic_uint64_t _nCountReadByte = 0;
		std::atomic_uint64_t _nCountWriteByte = 0;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/* порт ввода/вывода */
		const std::shared_ptr<iocp::CIocp> _pIocp;

		/* описатель */
		handle::CHandle _hAsyncHandle;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END