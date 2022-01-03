#pragma once

_WNAME_BEGIN
namespace io
{
	/** ������� ������� � �� */
	#define BUFFER_1K 1024
	#define BUFFER_2K 2048
	#define BUFFER_4K 4096
	#define BUFFER_8K 8192
	#define BUFFER_16K 16384
	#define BUFFER_32K 32768
	#define BUFFER_64K 65536

	/** ����������� ����/����� */
	class CAsyncIo : protected misc::CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		* @param pIocp - �������� �����/������.
		*/
		WNAME CAsyncIo(
			const std::shared_ptr<iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ������������� ����������� �������� �����/������.
		* @param hHandle - ���������.
		*/
		WNAME void bindHandle(
			handle::CHandle hHandle);
	//==========================================================================
		/**
		* �������� ������� ������������ ���������.
		* @return - ��������� ��������.
		*/
		WNAME bool isBindHandle() noexcept;
	//==========================================================================
		/**
		* �������� ���������.
		* @param hHandle - ���������.
		*/
		WNAME void changeHandle(
			handle::CHandle hHandle);
	//==========================================================================
		/**
		* ������� ���������.
		*/
		WNAME void closeHandle() noexcept;
	//==========================================================================
		/**
		* ����� ������������ ������.
		* @param bufferRead - ����� ��� ������
		* @param bufferSize - ������ ������ ��� ������.
		* @param offset - ����� ������������ ������.
		* @return - ��� ������.
		*/
		WNAME std::error_code startAsyncRead(
			const PBYTE bufferRead,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* ����� ������.
		* @param bufferRead - ����� ��� ������
		* @param bufferSize - ������ ������ ��� ������.
		* @param pdwReturnSize - ���������� ����������� ����.
		* @param offset - ����� ������������ ������.
		* @return - ��� ������.
		*/
		WNAME std::error_code startRead(
			const PBYTE bufferRead,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* ����� ����������� ������.
		* @param bufferWrite - ����� ��� ������
		* @param bufferSize - ������ ������ ��� ������.
		* @param offset - ����� ������������ ������.
		* @return - ��� ������.
		*/
		WNAME std::error_code startAsyncWrite(
			const PBYTE bufferWrite,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* ����� ������.
		* @param bufferWrite - ����� ��� ������
		* @param bufferSize - ������ ������ ��� ������.
		* @param pdwReturnSize - ���������� ���������� ����.
		* @param offset - ����� ������������ ������.
		* @return - ��� ������.
		*/
		WNAME std::error_code startWrite(
			const PBYTE bufferWrite,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* �������� ���������.
		* @return - ���������.
		*/
		WNAME HANDLE getHandle() noexcept;
	//==========================================================================
		/**
		* ��������� ������ � ��������� ���� ����������� ��������.
		*/
		WNAME void release() noexcept override;
	//==========================================================================
		/**
		* ����������.
		*/
		WNAME ~CAsyncIo();
	//==========================================================================
		CAsyncIo(const CAsyncIo&) = delete;
		CAsyncIo(CAsyncIo&&) = delete;
		CAsyncIo& operator=(const CAsyncIo&) = delete;
		CAsyncIo& operator=(CAsyncIo&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ������������ ������.
		* @param bufferRead - ����� ������.
		* @param dwReturnSize - ���������� ����������� ����.
		* @param ec - ��� ������ ����������.
		*/
		WNAME virtual void asyncReadComplitionHandler(
			const PBYTE bufferRead,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ����������� ������.
		* @param bufferWrite - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ������ ����������.
		*/
		WNAME virtual void asyncWriteComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������.
		* @param pAsyncOperation - ����������� ��������.
		*/
		WNAME static void asyncReadIocpHandler(
			iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������.
		* @param pAsyncOperation - ����������� ��������.
		*/
		WNAME static void asyncWriteIocpHandler(
			iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ��������.
		* @param pAsyncOperation - ����������� ��������.
		*/
		WNAME static void asyncIocpHandler(
			iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** ���������� */
		std::atomic_uint64_t _nCountReadByte = 0;
		std::atomic_uint64_t _nCountWriteByte = 0;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/* ���� �����/������ */
		const std::shared_ptr<iocp::CIocp> _pIocp;

		/* ��������� */
		handle::CHandle _hAsyncHandle;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END