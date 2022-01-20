#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** ���� ���� ����� ��� ��������� �����/������ */
	class WNAME CIocp::CThreadPoolWorker final : protected misc::CCounter
	{
		friend class CThreadPool;
		friend class CIocp;
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
		* ��������� ������ � ��������� ���� ����������� ��������.
		* @param bIsWait - ������� ��������.
		*/
		void release(
			const bool bIsWait) noexcept override;
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