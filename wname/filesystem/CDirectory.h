#pragma once

_WNAME_BEGIN
namespace filesystem
{
	/** работа с директорией */
	class CDirectory : protected io::CAsyncIo
	{
#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор папки.
		* @param directoryPath - абсолютное путь до папки.
		* @param pIocp - указатель на механизм ввода/вывода.
		*/
		WNAME CDirectory(
			const std::filesystem::path directoryPath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* конструктор папки.
		* @param directoryPath - абсолютное путь до папки.
		* @param pIocp - указатель на механизм ввода/вывода.
		* @oaram pParent - указатель на родительный класс.
		*/
		WNAME CDirectory(
			const std::filesystem::path directoryPath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp,
			CDirectory* const pParent);
	//==========================================================================
		/**
		* открыть папку.
		* @param isSubOpenDirectory - открыть все вложенные папки.
		* @param isNotify - признак посылки уведомлений.
		* @param dwDesiredAccess - запрошенный доступ к папки.
		* ( GENERIC_READ | GENERIC_WRITE).
		* @param dwShareMode - параметр общего доступа к папки.
		* (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE).
		* @param dwCreationDisposition - действие над папкой.
		* (CREATE_NEW | CREATE_ALWAYS | OPEN_EXISTING | OPEN_ALWAYS | TRUNCATE_EXISTING).
		* @param dwFlagsAndAttributes - атрибуты и флаги.
		* FILE_FLAG_BACKUP_SEMANTICS и FILE_FLAG_OVERLAPPED всегда присутствует.
		* @return - код ошибки.
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
		* получить путь.
		* @return - абсолютный путь до папки.
		*/
		WNAME std::filesystem::path getPath();
	//==========================================================================
		/**
		* открыта ли папка.
		* @return - успех операции.
		*/
		WNAME bool isOpen() noexcept;
	//==========================================================================
		/**
		* получить список путей файлов.
		* @param bSubFile - включить в список файлы под папок.
		* @param proximatePath - обрезать начальный путь по.
		* @return - список файлов.
		*/
		WNAME std::list<std::filesystem::path> getFileList(
			const bool bSubFile,
			const std::filesystem::path proximatePath = "");
	//==========================================================================
		/**
		* удалить папку.
		* @return - код ошибки.
		*/
		WNAME std::error_code deleteDirectory();
	//==========================================================================
		/**
		* удалить папку.
		* @param directoryPath - путь до папки.
		* @return - код ошибки.
		*/
		WNAME static std::error_code deleteDirectory(
			std::filesystem::path directoryPath) noexcept;
	//==========================================================================
		/**
		* закрыть папку.
		*/
		WNAME void close() noexcept;
	//==========================================================================
		/**
		* деструктор.
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
		* найти все элементы.
		* @return - код ошибки.
		*/
		WNAME std::error_code enumDirectory();
	//==========================================================================
		/**
		* добавление файла в список.
		* @param uId - id файла.
		* @param filePath - абсолютный путь.
		*/
		WNAME void addFileToDirectory(
			const std::filesystem::path& filePath);
	//==========================================================================
		/**
		* добавление папки в список.
		* @param uId - id папки.
		*/
		WNAME void addDirectoryToDirectory(
			const std::filesystem::path& directoryPath);
	//==========================================================================
		/**
		* удаление файла из списка.
		* @param uId - id удаляемого файла.
		*/
		WNAME void removeFileFromDirectory(
			const UINT64 uId) noexcept;
	//==========================================================================
		/**
		* удаление папки из списка.
		* @param uId - id удаляемой папки.
		*/
		WNAME void removeDirectoryFromDirectory(
			const UINT64 uId) noexcept;
	//==========================================================================
		/**
		* изменить количество вложенных файлов.
		* @param countItem - количество элементов
		*/
		WNAME void changeSubFileCount(
			const int countItem) noexcept;
	//==========================================================================
		/**
		* изменить количество вложенных папок.
		* @param countItem - количество элементов
		*/
		WNAME void changeSubDirectoryCount(
			const int countItem) noexcept;
	//==========================================================================
		/**
		* старт отслеживания папки.
		* @return - код ошибки.
		*/
		WNAME std::error_code startNotify();
	//==========================================================================
		/**
		* применение уведомлений.
		* @param pNotifyInfo - уведомления.
		*/
		WNAME void notifyAcceptCompilteHandler(
			PFILE_NOTIFY_EXTENDED_INFORMATION pNotifyInfo);
	//==========================================================================
		/**
		* уведомление об изменение.
		* @param pAsyncOperation - асинхронная операция.
		*/
		WNAME static void notifyCompilteHandler(
			io::iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* уведомление об изменение в файле.
		* @param dwAction - действие.
		* @param pFile - файл.
		* @param bIsSubItem - признак того что элемент является под элементом.
		*/
		WNAME virtual void notifyFileCompilteHandler(
			const DWORD dwAction,
			const std::shared_ptr<CFile> pFile,
			const bool bIsSubItem = false);
	//==========================================================================
		/**
		* уведомление об изменение в папке.
		* @param dwAction - действие.
		* @param pDirectory - папка.
		* @param bIsSubItem - признак того что элемент является под элементом.
		*/
		WNAME virtual void notifyDirectoryCompilteHandler(
			const DWORD dwAction,
			const std::shared_ptr<CDirectory> pDirectory,
			const bool bIsSubItem = false);
	//==========================================================================
		/**
		* уведомление об ошибки уведомления.
		* @param ec - код ошибки.
		* @param bIsSubItem - признак того что элемент является под элементом.
		* @return - признак перезапуска уведомлений. 
		*/
		WNAME virtual bool notifyErrorCompilteHandler(
			const std::error_code ec,
			const bool bIsSubItem = false) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** родительный класс */
		CDirectory* const _pParent;

		/** абсолютный путь до папки */
		std::filesystem::path _directoryPath;

		/** список всех файлов в папке */
		std::unordered_map<UINT64, std::shared_ptr<CFile>> _listFile;

		/** список всех папок в папке */
		std::unordered_map<UINT64, std::shared_ptr<CDirectory>> _listDirectory;

		/** признак открытия вложенных папок */
		bool _isSubOpenDirectory = false;

		/** признак посылки уведомлений */
		bool _isNotify = false;

		/** информация с уведомлением об изменении */
		std::vector<BYTE> _bufferNotify;

		/** количество входящих элементов */
		std::atomic_uint64_t _nCountSubFile;
		std::atomic_uint64_t _nCountSubDirectory;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END