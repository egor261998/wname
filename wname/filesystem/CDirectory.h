#pragma once

_WNAME_BEGIN
namespace filesystem
{
	/** ������ � ����������� */
	class CDirectory : protected io::CAsyncIo
	{
#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �����.
		* @param directoryPath - ���������� ���� �� �����.
		* @param pIocp - ��������� �� �������� �����/������.
		*/
		WNAME CDirectory(
			const std::filesystem::path directoryPath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ����������� �����.
		* @param directoryPath - ���������� ���� �� �����.
		* @param pIocp - ��������� �� �������� �����/������.
		* @oaram pParent - ��������� �� ����������� �����.
		*/
		CDirectory(
			const std::filesystem::path directoryPath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp,
			CDirectory* const pParent);
	//==========================================================================
		/**
		* ������� �����.
		* @param isSubOpenDirectory - ������� ��� ��������� �����.
		* @param isNotify - ������� ������� �����������.
		* @param dwDesiredAccess - ����������� ������ � �����.
		* ( GENERIC_READ | GENERIC_WRITE).
		* @param dwShareMode - �������� ������ ������� � �����.
		* (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE).
		* @param dwCreationDisposition - �������� ��� ������.
		* (CREATE_NEW | CREATE_ALWAYS | OPEN_EXISTING | OPEN_ALWAYS | TRUNCATE_EXISTING).
		* @param dwFlagsAndAttributes - �������� � �����.
		* FILE_FLAG_BACKUP_SEMANTICS � FILE_FLAG_OVERLAPPED ������ ������������.
		* @return - ��� ������.
		*/
		WNAME std::error_code createDirectory(
			const bool isSubOpenDirectory = false,
			const bool isNotify = false,
			const DWORD dwDesiredAccess = 0,
			const DWORD dwShareMode = 0,
			const DWORD dwCreationDisposition = 0,
			DWORD dwFlagsAndAttributes = 0);
	//==========================================================================
		/**
		* �������� ����.
		* @return - ���������� ���� �� �����.
		*/
		WNAME std::filesystem::path getPath();
	//==========================================================================
		/**
		* ������� �� �����.
		* @return - ����� ��������.
		*/
		WNAME bool isOpen() noexcept;
	//==========================================================================
		/**
		* �������� ������ ����� ������.
		* @param bSubFile - �������� � ������ ����� ��� �����.
		* @param proximatePath - �������� ��������� ���� ��.
		* @return - ������ ������.
		*/
		WNAME std::list<std::filesystem::path> getFileList(
			const bool bSubFile,
			const std::filesystem::path proximatePath = "");
	//==========================================================================
		/**
		* ������� �����.
		* @return - ��� ������.
		*/
		WNAME std::error_code deleteDirectory();
	//==========================================================================
		/**
		* ������� �����.
		* @param directoryPath - ���� �� �����.
		* @return - ��� ������.
		*/
		WNAME static std::error_code deleteDirectory(
			std::filesystem::path directoryPath) noexcept;
	//==========================================================================
		/**
		* ������� �����.
		*/
		WNAME void close() noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		WNAME ~CDirectory();
	//==========================================================================
		CDirectory(const CDirectory&) = delete;
		CDirectory(CDirectory&&) = delete;
		CDirectory& operator=(const CDirectory&) = delete;
		CDirectory& operator=(CDirectory&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* ����� ��� ��������.
		* @return - ��� ������.
		*/
		std::error_code enumDirectory();
	//==========================================================================
		/**
		* ���������� ����� � ������.
		* @param uId - id �����.
		* @param filePath - ���������� ����.
		*/
		void addFileToDirectory(
			const std::filesystem::path& filePath);
	//==========================================================================
		/**
		* ���������� ����� � ������.
		* @param uId - id �����.
		*/
		void addDirectoryToDirectory(
			const std::filesystem::path& directoryPath);
	//==========================================================================
		/**
		* �������� ����� �� ������.
		* @param uId - id ���������� �����.
		*/
		void removeFileFromDirectory(
			const UINT64 uId) noexcept;
	//==========================================================================
		/**
		* �������� ����� �� ������.
		* @param uId - id ��������� �����.
		*/
		void removeDirectoryFromDirectory(
			const UINT64 uId) noexcept;
	//==========================================================================
		/**
		* �������� ���������� ��������� ������.
		* @param countItem - ���������� ���������
		*/
		void changeSubFileCount(
			const int countItem) noexcept;
	//==========================================================================
		/**
		* �������� ���������� ��������� �����.
		* @param countItem - ���������� ���������
		*/
		void changeSubDirectoryCount(
			const int countItem) noexcept;
	//==========================================================================
		/**
		* ����� ������������ �����.
		* @return - ��� ������.
		*/
		std::error_code startNotify();
	//==========================================================================
		/**
		* ���������� �����������.
		* @param pNotifyInfo - �����������.
		*/
		void notifyAcceptCompilteHandler(
			PFILE_NOTIFY_EXTENDED_INFORMATION pNotifyInfo) noexcept;
	//==========================================================================
		/**
		* ����������� �� ���������.
		* @param pAsyncOperation - ����������� ��������.
		*/
		static void notifyCompilteHandler(
			io::iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* ����������� �� ��������� � �����.
		* @param dwAction - ��������.
		* @param pFile - ����.
		* @param bIsSubItem - ������� ���� ��� ������� �������� ��� ���������.
		*/
		virtual void notifyFileCompilteHandler(
			const DWORD dwAction,
			const std::shared_ptr<CFile> pFile,
			const bool bIsSubItem = false);
	//==========================================================================
		/**
		* ����������� �� ��������� � �����.
		* @param dwAction - ��������.
		* @param pDirectory - �����.
		* @param bIsSubItem - ������� ���� ��� ������� �������� ��� ���������.
		*/
		virtual void notifyDirectoryCompilteHandler(
			const DWORD dwAction,
			const std::shared_ptr<CDirectory> pDirectory,
			const bool bIsSubItem = false);
	//==========================================================================
		/**
		* ����������� �� ������ �����������.
		* @param ec - ��� ������.
		* @param bIsSubItem - ������� ���� ��� ������� �������� ��� ���������.
		*/
		virtual void notifyErrorCompilteHandler(
			const std::error_code ec,
			const bool bIsSubItem = false) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ����������� ����� */
		CDirectory* const _pParent;

		/** ���������� ���� �� ����� */
		std::filesystem::path _directoryPath;

		/** ������ ���� ������ � ����� */
		std::unordered_map<UINT64, std::shared_ptr<CFile>> _listFile;

		/** ������ ���� ����� � ����� */
		std::unordered_map<UINT64, std::shared_ptr<CDirectory>> _listDirectory;

		/** ������� �������� ��������� ����� */
		bool _isSubOpenDirectory = false;

		/** ������� ������� ����������� */
		bool _isNotify = false;

		std::vector<BYTE> _bufferNotify;
		///** ���������� � ������������ �� ���������, ������ ������ � 64�� */
		//union
		//{
		//	FILE_NOTIFY_EXTENDED_INFORMATION _notifyInfo;
		//	char _bufferNotifyInfo[BUFFER_64K];
		//};

		/** ���������� �������� ��������� */
		std::atomic_uint64_t _nCountSubFile;
		std::atomic_uint64_t _nCountSubDirectory;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END