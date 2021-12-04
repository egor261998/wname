#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** нить пула нитей для механизма ввода/вывода */
	class CIocp::CThreadPoolWorker : public misc::CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* конструктор нити.
		* @param pThreadPool - пул нитей.
		*/
		CThreadPoolWorker(
			CThreadPool* const pThreadPool);
	//==========================================================================
		/**
		* запустить новую нить.
		* @param pThreadPool - пул нитей.
		*/
		static void start(
			CThreadPool* const pThreadPool);
	//==========================================================================
		/**
		* закончить работу и дождаться всех операций.
		*/
		void release() noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		~CThreadPoolWorker();
	//==========================================================================
		CThreadPoolWorker(const CThreadPoolWorker&) = delete;
		CThreadPoolWorker(CThreadPoolWorker&&) = delete;
		CThreadPoolWorker& operator=(const CThreadPoolWorker&) = delete;
		CThreadPoolWorker& operator=(CThreadPoolWorker&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** пул нитей */
		CThreadPool* const _pThreadPool;

		/** нить */
		std::thread _thread;

		/** признак использования нити */
		bool _bIsBusy = false;
		/** признак высвобождения нити */
		bool _bIsFree = false;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END