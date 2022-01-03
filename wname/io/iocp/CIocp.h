#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** ���������� ��������� Iocp */
	class CIocp final : protected misc::CCounter
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** ����������� �������� */
		class CAsyncOperation;
	//==========================================================================
		/**
		* �������� ������� ���������� ��������.
		* @param pAsyncOperation - ����������� ��������.
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
		* ������������� Iocp.
		* @param pLogger - ������ �����������.
		*/
		WNAME CIocp(
			const std::shared_ptr<logger::ILogger> pLogger = nullptr) noexcept(false);
	//==========================================================================
		/**
		* ������������� Iocp.
		* @param minThreadCount - ����������� ���-�� ����� � ����.
		* @param maxThreadCount - ������������ ���-�� ����� � ����.
		* @param pLogger - ������ �����������.
		*/
		WNAME CIocp(
			const DWORD minThreadCount = 1,
			const DWORD maxThreadCount = std::thread::hardware_concurrency(),
			const std::shared_ptr<logger::ILogger> pLogger = nullptr) noexcept(false);
	//==========================================================================
		/**
		* ��������� ���������� � ����� �����/������.
		* @param hHandle - ������������� ����������.
		* @param ulpCompletionKey - ���� ����������.
		*/
		WNAME void bind(
			const HANDLE hHandle,
			const ULONG_PTR ulCompletionKey = 0);
	//==========================================================================
		/**
		* �������� ����������� ��������.
		* @param pCompletionRoutineContext - �������� ����������� ��������.
		* @param fComplitionRoutine - ������� ���������� ����������� ��������.
		* @return - ����������� ��������.
		*/
		WNAME CAsyncOperation* getAsyncOperation(
			const PVOID pCompletionRoutineContext = nullptr,
			const FAsyncCompletion fComplitionRoutine = nullptr);
	//==========================================================================
		/**
		* ������� ���������� ����� ����� ���� �����/������.
		* @param pCompletionRoutineContext - �������� ����������� ��������.
		* @param fComplitionRoutine - ������� ���������� ����������� ��������.
		* @param ulComplitionKey - ���� ���������.
		*/
		WNAME void transit(
			const PVOID pCompletionRoutineContext,
			const FAsyncCompletion fComplitionRoutine,
			const ULONG_PTR ulComplitionKey = 0);
	//==========================================================================
		/*
		* ��� ���������.
		* @param eMessageType - ��� ���������.
		* @param ex - ����������.
		* @param ec - ��� ������.
		* @param messageFunction - ������� �� ������� ��������� ���������.
		*/
		WNAME void log(
			const logger::EMessageType eMessageType,
			const std::exception& ex,
			const std::error_code ec = std::error_code(),
			const char* const messageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* ��� ���������.
		* @param eMessageType - ��� ���������.
		* @param message - ���������.
		* @param ec - ��� ������.
		* @param messageFunction - ������� �� ������� ��������� ���������.
		*/
		WNAME void log(
			const logger::EMessageType eMessageType,
			const wchar_t* const message,
			const std::error_code ec = std::error_code(),
			const char* const messageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/**
		* ���������� ��������� ���� ���������� ��� ���� HANDLE.
		* @rerutn - ��������� Iocp.
		*/
		WNAME operator HANDLE() noexcept;
	//==========================================================================
		/**
		* ��������� ������.
		*/
		void release() noexcept override;
	//==========================================================================
		/** 
		* ���������� ��������� IOCP 
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
		* ���������� ����������� �������� �������� ����� �����/������.
		* @param pWorkerThread - ������ ���� �����������.
		*/
		WNAME void workerThread(
			const std::shared_ptr<CThreadPoolWorker> pWorkerThread) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ����� �������� �������� �� ���� � (��),
			����� ����� ������� ��� ���� �������� */
		const DWORD _nWorkerTimeIde = 10000;

		/** ����������������� ����������� ���-�� ��������� ����������� ��������,
			�������������� � ������� */
		const DWORD _nMinReservAsyncOperation = 10;

		/** ����������������� ������������ ���-�� ��������� ����������� ��������,
			�������������� � ������� */
		const DWORD _nMaxReservAsyncOperation = 100;

		/** ���� �����/������ */
		handle::CHandle _hIocp;

		/** ��� ����� */
		std::unique_ptr<CThreadPool> _pThreadPool;

		/** TLS ��� �������� ��������� �����/������ */
		misc::CThreadTls<CThreadPoolWorker> _tls;

		/** ���������� ����������� �������� */
		std::atomic_bool _updateOperation = false;

		/** ������ ����������� �������� */
		misc::CInterlockedList<CAsyncOperation> _listFreeAsyncOperation;

		/** ������ ����������� */
		std::shared_ptr<logger::ILogger> _pLogger;
	//==========================================================================
	#pragma endregion
	};

	using CAsyncOperation = CIocp::CAsyncOperation;
}
_WNAME_END