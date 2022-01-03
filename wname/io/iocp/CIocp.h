#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** реализация механизма Iocp */
	class CIocp final : protected misc::CCounter
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** асинхронная операция */
		class CAsyncOperation;
	//==========================================================================
		/**
		* прототип функции завершения операции.
		* @param pAsyncOperation - асинхронная операция.
		*/
		typedef void (*FAsyncCompletion)(
			CAsyncOperation* const pAsyncOperation);
	//==========================================================================
	#pragma endregion

	#pragma region Private_Inner
	private:
	//==========================================================================
		class CThreadPool;
		class CThreadPoolWorker;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* инициализация Iocp.
		* @param pLogger - объект логирования.
		*/
		WNAME CIocp(
			const std::shared_ptr<logger::ILogger> pLogger = nullptr) noexcept(false);
	//==========================================================================
		/**
		* инициализация Iocp.
		* @param minThreadCount - минимальное кол-во нитей в пуле.
		* @param maxThreadCount - максимальное кол-во нитей в пуле.
		* @param pLogger - объект логирования.
		*/
		WNAME CIocp(
			const DWORD minThreadCount = 1,
			const DWORD maxThreadCount = std::thread::hardware_concurrency(),
			const std::shared_ptr<logger::ILogger> pLogger = nullptr) noexcept(false);
	//==========================================================================
		/**
		* привязать дескриптор к порту ввода/вывода.
		* @param hHandle - привязываемый дескриптор.
		* @param ulpCompletionKey - ключ завершения.
		*/
		WNAME void bind(
			const HANDLE hHandle,
			const ULONG_PTR ulCompletionKey = 0);
	//==========================================================================
		/**
		* получить асинхронную операцию.
		* @param pCompletionRoutineContext - контекст асинхронной операции.
		* @param fComplitionRoutine - функция завершения асинхронной операции.
		* @return - асинхронная операция.
		*/
		WNAME CAsyncOperation* getAsyncOperation(
			const PVOID pCompletionRoutineContext = nullptr,
			const FAsyncCompletion fComplitionRoutine = nullptr);
	//==========================================================================
		/**
		* вызвать транзитный вызов через порт ввода/вывода.
		* @param pCompletionRoutineContext - контекст асинхронной операции.
		* @param fComplitionRoutine - функция завершения асинхронной операции.
		* @param ulComplitionKey - ключ заверения.
		*/
		WNAME void transit(
			const PVOID pCompletionRoutineContext,
			const FAsyncCompletion fComplitionRoutine,
			const ULONG_PTR ulComplitionKey = 0);
	//==========================================================================
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param ex - исключение.
		* @param ec - код ошибки.
		* @param messageFunction - функция из которой вызвалось сообщение.
		*/
		WNAME void log(
			const logger::EMessageType eMessageType,
			const std::exception& ex,
			const std::error_code ec = std::error_code(),
			const char* const messageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param message - сообщение.
		* @param ec - код ошибки.
		* @param messageFunction - функция из которой вызвалось сообщение.
		*/
		WNAME void log(
			const logger::EMessageType eMessageType,
			const wchar_t* const message,
			const std::error_code ec = std::error_code(),
			const char* const messageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/**
		* перегрузка оператора пере присвоения для типа HANDLE.
		* @rerutn - описатель Iocp.
		*/
		WNAME operator HANDLE() noexcept;
	//==========================================================================
		/**
		* закончить работу.
		*/
		void release() noexcept override;
	//==========================================================================
		/** 
		* деструктор механизма IOCP 
		*/
		WNAME ~CIocp();
	//==========================================================================
		CIocp(const CIocp&) = delete;
		CIocp(CIocp&&) = delete;
		CIocp& operator=(const CIocp&) = delete;
		CIocp& operator=(CIocp&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* обработчик асинхронных операций текущего порта ввода/вывода.
		* @param pWorkerThread - объект нити обработчика.
		*/
		WNAME void workerThread(
			const std::shared_ptr<CThreadPoolWorker> pWorkerThread) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** время ожидания нагрузки на нить в (мс),
			после этого времени она сама удалится */
		const DWORD _nWorkerTimeIde = 10000;

		/** зарезервированное минимальное кол-во свободных асинхронных операций,
			присутствующих в системе */
		const DWORD _nMinReservAsyncOperation = 10;

		/** зарезервированное максимальное кол-во свободных асинхронных операций,
			присутствующих в системе */
		const DWORD _nMaxReservAsyncOperation = 100;

		/** порт ввода/вывода */
		handle::CHandle _hIocp;

		/** пул нитей */
		std::unique_ptr<CThreadPool> _pThreadPool;

		/** TLS для текущего механизма ввода/вывода */
		misc::CThreadTls<CThreadPoolWorker> _tls;

		/** обновление асинхронных операций */
		std::atomic_bool _updateOperation = false;

		/** список асинхронных операций */
		misc::CInterlockedList<CAsyncOperation> _listFreeAsyncOperation;

		/** объект логирования */
		std::shared_ptr<logger::ILogger> _pLogger;
	//==========================================================================
	#pragma endregion
	};

	using CAsyncOperation = CIocp::CAsyncOperation;
}
_WNAME_END