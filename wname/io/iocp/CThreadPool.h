#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** пул нитей для механизма ввода/вывода */
	class CIocp::CThreadPool final : protected misc::CCounter
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
		WNAME CThreadPool(
			CIocp* const pIocp,
			const DWORD minThreadCount,
			const DWORD maxThreadCount);
	//==========================================================================
		/**
		* высвободить память из под нитки.
		* @param pThreadPoolWorker - нить пула.
		*/
		WNAME void delWorker(
			const CThreadPoolWorker* const pThreadPoolWorker) noexcept;
	//==========================================================================
		/**
		* попытка пометить нитку для удаления.
		* @param pThreadPoolWorker - нить пула.
		* @return - успех операции.
		*/
		WNAME bool canFreeWorker(
			CThreadPoolWorker* const pThreadPoolWorker) noexcept;
	//==========================================================================
		/**
		* попытка добавить новую нитку.
		* @param bAddAnyway - добавить нить в любом случае.
		* @return - успех операции.
		*/
		WNAME bool canAddWorker(
			bool bAddAnyway = false);
	//==========================================================================
		/**
		* снизить занятость пула.
		*/
		WNAME void decrementBusyWorker() noexcept;
	//==========================================================================
		/**
		* закончить работу.
		*/
		WNAME void release() noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CThreadPool();
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