#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** ��� ����� ��� ��������� �����/������ */
	class CIocp::CThreadPool : public misc::CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ������������� ���� �����.
		* @param pIocp - ��������� �� ����������� �����.
		* @param minThreadCount - ����������� ���-�� ����� � ����.
		* @param maxThreadCount - ������������ ���-�� ����� � ����.
		*/
		CThreadPool(
			CIocp* const pIocp,
			const DWORD minThreadCount,
			const DWORD maxThreadCount);
	//==========================================================================
		/**
		* ����������� ������ �� ��� �����.
		* @param pThreadPoolWorker - ���� ����.
		*/
		void delWorker(
			const CThreadPoolWorker* const pThreadPoolWorker) noexcept;
	//==========================================================================
		/**
		* ������� �������� ����� ��� ��������.
		* @param pThreadPoolWorker - ���� ����.
		* @return - ����� ��������.
		*/
		bool canFreeWorker(
			CThreadPoolWorker* const pThreadPoolWorker) noexcept;
	//==========================================================================
		/**
		* ������� �������� ����� �����.
		* @param bAddAnyway - �������� ���� � ����� ������.
		* @return - ����� ��������.
		*/
		bool canAddWorker(
			bool bAddAnyway = false);
	//==========================================================================
		/**
		* ������� ��������� ����.
		*/
		void decrementBusyWorker() noexcept;
	//==========================================================================
		/**
		* ��������� ������ � ��������� ���� ��������.
		*/
		void release() noexcept override;
	//==========================================================================
		/**
		* ����������.
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
		/** ��������� �� ���� �����/������ */
		CIocp* const _pIocp;

		/** ����������� ���-�� ����� */
		DWORD _nMinThreadCount = 0;
		/** ������������ ���-�� ����� */
		DWORD _nMaxThreadCount = 0;
		/** ������� ���-�� ����� */
		DWORD _nCurrentThreadCount = 0;
		/** ������������ ���-�� ����� */
		DWORD _nBusyThreadCount = 0;
		/** �������������� ���-�� ����� */
		DWORD _nFreeThreadCount = 0;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END