#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** асинхронная операция механизма ввода/вывода */
	class CIocp::CAsyncOperation : private misc::CInterlockedItem
	{
		friend class CIocp;
		friend class misc::CInterlockedList<CAsyncOperation>;

	#pragma region Public_Inner
	//==========================================================================
		/** буфер ввода/вывода */
		typedef struct _SBuffer
		{
			/** размер буфера */
			DWORD _dwSize;
			/** буфер который используется в операции */
			PBYTE _p;		
		} SBuffer, *PSBuffer;
	//==========================================================================

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор асинхронной операции.
		* @param pIocp - указатель на порт ввода/вывода.
		* @param pCompletionRoutineContext - контекст асинхронной операции.
		* @param fComplitionRoutine - функция завершения асинхронной операции.
		*/
		WNAME CAsyncOperation(
			CIocp* const pIocp,
			const PVOID pCompletionRoutineContext = nullptr,
			const FAsyncCompletion fComplitionRoutine = nullptr);
	//==========================================================================
		/**
		* отмена асинхронной операции.
		*/
		WNAME void cancel() noexcept;
	//==========================================================================
		CAsyncOperation(const CAsyncOperation&) = delete;
		CAsyncOperation(CAsyncOperation&&) = delete;
		CAsyncOperation& operator=(const CAsyncOperation&) = delete;
		CAsyncOperation& operator=(CAsyncOperation&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** указатель на порт ввода/вывода */
		CIocp* const _pIocp;

		/** ключ завершения */
		ULONG_PTR _ulCompletionKey = 0;

		/** асинхронный контекст */
		OVERLAPPED _overlapped = { 0 };

		union
		{
			/** буфер для сокетов */
			WSABUF _wsaBuffer{ 0 };

			/** буфер для всех */
			SBuffer _buffer;
		};	
		
		/** возвращенных байт из асинхронной операции */
		DWORD _dwReturnSize = 0;
		/** код ошибки */
		std::error_code _ec;

		/** контекст функции завершения */
		PVOID _pCompletionRoutineContext;
		/** функция завершения */
		FAsyncCompletion _fComplitionRoutine;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** нить вызова, если она из пула */
		CThreadPoolWorker* _pThreadPoolWorker;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END