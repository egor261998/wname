#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** пул нитей для механизма ввода/вывода */
	class WNAME CIocp::CThreadPool final : protected misc::CCounter
	{
		friend class CThreadPoolWorker;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* инициализация пула нитей.
		* @param pIocp - указатель на родительный класс.
		* @param minThreadCount - минимальное кол-во нитей в пуле.
		* @param maxThreadCount - максимальное кол-во нитей в пуле.
		*/
		CThreadPool(
			CIocp* const pIocp,
			const DWORD minThreadCount,
			const DWORD maxThreadCount);
	//==========================================================================
		/**
		* высвободить память из под нитки.
		* @param pThreadPoolWorker - нить пула.
		*/
		void delWorker(
			const CThreadPoolWorker* const pThreadPoolWorker) noexcept;
	//==========================================================================
		/**
		* попытка пометить нитку для удаления.
		* @param pThreadPoolWorker - нить пула.
		* @return - успех операции.
		*/
		bool canFreeWorker(
			CThreadPoolWorker* const pThreadPoolWorker) noexcept;
	//==========================================================================
		/**
		* попытка добавить новую нитку.
		* @param bAddAnyway - добавить нить в любом случае.
		* @return - успех операции.
		*/
		bool canAddWorker(
			bool bAddAnyway = false);
	//==========================================================================
		/**
		* снизить занятость пула.
		*/
		void decrementBusyWorker() noexcept;
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
		~CThreadPool();
	//==========================================================================
		CThreadPool(const CThreadPool&) = delete;
		CThreadPool(CThreadPool&&) = delete;
		CThreadPool& operator=(const CThreadPool&) = delete;
		CThreadPool& operator=(CThreadPool&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	public:
	//==========================================================================
		/** указатель на порт ввода/вывода */
		CIocp* const _pIocp;

		/** минимальное кол-во нитей */
		DWORD _nMinThreadCount = 0;
		/** максимальное кол-во нитей */
		DWORD _nMaxThreadCount = 0;
		/** текущее кол-во нитей */
		DWORD _nCurrentThreadCount = 0;
		/** используемое кол-во нитей */
		DWORD _nBusyThreadCount = 0;
		/** высвобождаемое кол-во нитей */
		DWORD _nFreeThreadCount = 0;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END