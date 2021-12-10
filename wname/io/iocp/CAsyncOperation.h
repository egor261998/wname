#pragma once

_WNAME_BEGIN
namespace io::iocp
{
	/** ����������� �������� ��������� �����/������ */
	class CIocp::CAsyncOperation : private misc::CInterlockedItem
	{
		friend class CIocp;
		friend class misc::CInterlockedList<CAsyncOperation>;

	#pragma region Public_Inner
	//==========================================================================
		/** ����� �����/������ */
		typedef struct _SBuffer
		{
			/** ������ ������ */
			DWORD _dwSize;
			/** ����� ������� ������������ � �������� */
			PBYTE _p;		
		} SBuffer, *PSBuffer;
	//==========================================================================

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ����������� ��������.
		* @param pIocp - ��������� �� ���� �����/������.
		* @param pCompletionRoutineContext - �������� ����������� ��������.
		* @param fComplitionRoutine - ������� ���������� ����������� ��������.
		*/
		WNAME CAsyncOperation(
			CIocp* const pIocp,
			const PVOID pCompletionRoutineContext = nullptr,
			const FAsyncCompletion fComplitionRoutine = nullptr);
	//==========================================================================
		/**
		* ������ ����������� ��������.
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
		/** ��������� �� ���� �����/������ */
		CIocp* const _pIocp;

		/** ���� ���������� */
		ULONG_PTR _ulCompletionKey = 0;

		/** ����������� �������� */
		OVERLAPPED _overlapped = { 0 };

		union
		{
			/** ����� ��� ������� */
			WSABUF _wsaBuffer{ 0 };

			/** ����� ��� ���� */
			SBuffer _buffer;
		};	
		
		/** ������������ ���� �� ����������� �������� */
		DWORD _dwReturnSize = 0;
		/** ��� ������ */
		std::error_code _ec;

		/** �������� ������� ���������� */
		PVOID _pCompletionRoutineContext;
		/** ������� ���������� */
		FAsyncCompletion _fComplitionRoutine;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ���� ������, ���� ��� �� ���� */
		CThreadPoolWorker* _pThreadPoolWorker;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END