#pragma once

_WNAME_BEGIN
namespace filesystem
{
	/** ������ � ������ */
	class WNAME CFile : protected io::CAsyncIo
	{
		friend class CDirectory;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �����.
		* @param filePath - ���� �� �����.
		* @param pIocp - �������� �����/������.
		*/
		CFile(
			const std::filesystem::path filePath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ������ �� ����.
		* @return - ����� ��������.
		*/
		bool isOpen() noexcept;
	//==========================================================================
		/**
		* ������� ��� ������� ����.
		* @param dwDesiredAccess - ����������� ������ � �����.
		* ( GENERIC_READ | GENERIC_WRITE).
		* @param dwShareMode - �������� ������ ������� � �����.
		* (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE).
		* @param dwCreationDisposition - �������� ��� ������.
		* (CREATE_NEW | CREATE_ALWAYS | OPEN_EXISTING | OPEN_ALWAYS | TRUNCATE_EXISTING).
		* @param dwFlagsAndAttributes - �������� � �����.
		* FILE_FLAG_OVERLAPPED ������ ������������.
		* @return - ��� ������.
		*/
		std::error_code createFile(
			const DWORD dwDesiredAccess = 0,
			const DWORD dwShareMode = 0,
			const DWORD dwCreationDisposition = 0,
			const DWORD dwFlagsAndAttributes = 0);
	//==========================================================================
		/**
		* ����� ������������ ������ �� �����.
		* @param bufferRead - ����� ��� ������
		* @param bufferSize - ������ ������ ��� ������.
		* @param offset - ����� ������������ ������.
		* @return - ��� ������.
		*/
		std::error_code startAsyncReadFile(
			const PBYTE bufferRead,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* ����� ������ �� �����.
		* @param bufferRead - ����� ��� ������
		* @param bufferSize - ������ ������ ��� ������.
		* @param pdwReturnSize - ���������� ����������� ����.
		* @param offset - ����� ������������ ������.
		* @return - ��� ������.
		*/
		std::error_code startReadFile(
			const PBYTE bufferRead,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* ����� ����������� ������ � ����.
		* @param bufferWrite - ����� ��� ������
		* @param bufferSize - ������ ������ ��� ������.
		* @param offset - ����� ������������ ������.
		* @return - ��� ������.
		*/
		std::error_code startAsyncWriteFile(
			const PBYTE bufferWrite,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* ����� ������ � ����.
		* @param bufferWrite - ����� ��� ������
		* @param bufferSize - ������ ������ ��� ������.
		* @param pdwReturnSize - ���������� ���������� ����.
		* @param offset - ����� ������������ ������.
		* @return - ��� ������.
		*/
		std::error_code startWriteFile(
			const PBYTE bufferWrite,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* ������� ����.
		*/
		void close() noexcept;
	//==========================================================================
		/**
		* �������� ������ �����.
		* @param uSize - ������ �����.
		* @return - ��� ������.
		*/
		std::error_code getFileSize(
			UINT64 &uSize);
	//==========================================================================
		/**
		* ������� ����.
		* @return - ��� ������.
		*/
		std::error_code deleteFile();
	//==========================================================================
		/**
		* �������� ����.
		* @return - ��� ������.
		*/
		std::error_code clearFile() noexcept;
	//==========================================================================
		/**
		* �������� ���������� ���� �� �����.
		* @return - ���������� ���� �� �����.
		*/
		std::filesystem::path getPath();
	//==========================================================================
		/**
		* ������� ����.
		* @param filePath - ���� �� �����.
		* @return - ��� ������.
		*/
		static std::error_code deleteFile(
			const std::filesystem::path filePath) noexcept;
	//==========================================================================
		/**
		* ��������� ������.
		* @param bIsWait - ������� ��������.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* ����������.
		*/
		~CFile();
	//==========================================================================
		CFile(const CFile&) = delete;
		CFile(CFile&&) = delete;
		CFile& operator=(const CFile&) = delete;
		CFile& operator=(CFile&&) = delete;
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
		virtual void asyncReadFileComplitionHandler(
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
		virtual void asyncWriteFileComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������.
		* @param bufferRead - ����� ������.
		* @param dwReturnSize - ���������� ����������� ����.
		* @param ec - ��� ������ ����������.
		*/
		void asyncReadComplitionHandler(
			const PBYTE bufferRead,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������.
		* @param bufferWrite - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ������ ����������.
		*/
		void asyncWriteComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ���������� ���� �� ����� */
		std::filesystem::path _filePath;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END