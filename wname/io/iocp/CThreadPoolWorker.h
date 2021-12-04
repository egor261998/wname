#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** ���� ���� ����� ��� ��������� �����/������ */
	class CIocp::CThreadPoolWorker : public misc::CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* ����������� ����.
		* @param pThreadPool - ��� �����.
		*/
		CThreadPoolWorker(
			CThreadPool* const pThreadPool);
	//==========================================================================
		/**
		* ��������� ����� ����.
		* @param pThreadPool - ��� �����.
		*/
		static void start(
			CThreadPool* const pThreadPool);
	//==========================================================================
		/**
		* ��������� ������ � ��������� ���� ��������.
		*/
		void release() noexcept override;
	//==========================================================================
		/**
		* ����������.
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
		/** ��� ����� */
		CThreadPool* const _pThreadPool;

		/** ���� */
		std::thread _thread;

		/** ������� ������������� ���� */
		bool _bIsBusy = false;
		/** ������� ������������� ���� */
		bool _bIsFree = false;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END