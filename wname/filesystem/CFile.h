#pragma once

_WNAME_BEGIN
namespace filesystem
{
	/** работа с файлом */
	class CFile : protected io::CAsyncIo
	{
		friend class CDirectory;

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор файла.
		* @param filePath - путь до файла.
		* @param pIocp - механизм ввода/вывода.
		*/
		WNAME CFile(
			const std::filesystem::path filePath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* создать или открыть файл.
		* @param dwDesiredAccess - запрошенный доступ к файлу.
		* ( GENERIC_READ | GENERIC_WRITE).
		* @param dwShareMode - параметр общего доступа к файлу.
		* (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE).
		* @param dwCreationDisposition - действие над файлом.
		* (CREATE_NEW | CREATE_ALWAYS | OPEN_EXISTING | OPEN_ALWAYS | TRUNCATE_EXISTING).
		* @param dwFlagsAndAttributes - атрибуты и флаги.
		* FILE_FLAG_OVERLAPPED всегда присутствует.
		* @return - код ошибки.
		*/
		WNAME std::error_code createFile(
			const DWORD dwDesiredAccess = 0,
			const DWORD dwShareMode = 0,
			const DWORD dwCreationDisposition = 0,
			DWORD dwFlagsAndAttributes = 0);
	//==========================================================================
		/**
		* старт асинхронного чтения из файла.
		* @param bufferRead - буфер для чтения
		* @param bufferSize - размер буфера для чтения.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		WNAME std::error_code startAsyncReadFile(
			const PBYTE bufferRead,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* старт чтения из файла.
		* @param bufferRead - буфер для чтения
		* @param bufferSize - размер буфера для чтения.
		* @param pdwReturnSize - количество прочитанных байт.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		WNAME std::error_code startReadFile(
			const PBYTE bufferRead,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* старт асинхронной записи в файл.
		* @param bufferWrite - буфер для записи
		* @param bufferSize - размер буфера для записи.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		WNAME std::error_code startAsyncWriteFile(
			const PBYTE bufferWrite,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* старт записи в файл.
		* @param bufferWrite - буфер для записи
		* @param bufferSize - размер буфера для записи.
		* @param pdwReturnSize - количество записанных байт.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		WNAME std::error_code startWriteFile(
			const PBYTE bufferWrite,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const UINT64 offset = 0);
	//==========================================================================
		/**
		* закрыть файл.
		*/
		WNAME void close() noexcept;
	//==========================================================================
		/**
		* открыт ли файл.
		* @return - успех операции.
		*/
		WNAME bool isOpen() noexcept;
	//==========================================================================
		/**
		* получить размер файла.
		* @param uSize - размер файла.
		* @return - код ошибки.
		*/
		WNAME std::error_code getFileSize(
			UINT64 &uSize);
	//==========================================================================
		/**
		* удалить файл.
		* @return - код ошибки.
		*/
		WNAME std::error_code deleteFile();
	//==========================================================================
		/**
		* очистить файл.
		* @return - код ошибки.
		*/
		WNAME std::error_code clearFile() noexcept;
	//==========================================================================
		/**
		* получить абсолютный путь до файла.
		* @return - абсолютный путь до файла.
		*/
		WNAME std::filesystem::path getPath();
	//==========================================================================
		/**
		* удалить файл.
		* @param filePath - путь до файла.
		* @return - код ошибки.
		*/
		WNAME static std::error_code deleteFile(
			std::filesystem::path filePath) noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CFile();
	//==========================================================================
		CFile(const CFile&) = delete;
		CFile(CFile&&) = delete;
		CFile& operator=(const CFile&) = delete;
		CFile& operator=(CFile&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* обработчик события завершения асинхронного чтения.
		* @param bufferRead - буфер данных.
		* @param dwReturnSize - количество прочитанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME void asyncReadComplitionHandler(
			const PBYTE bufferRead,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи.
		* @param bufferWrite - буфер данных.
		* @param dwReturnSize - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME void asyncWriteComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронного чтения.
		* @param bufferRead - буфер данных.
		* @param dwReturnSize - количество прочитанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME virtual void asyncReadFileComplitionHandler(
			const PBYTE bufferRead,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронной записи.
		* @param bufferWrite - буфер данных.
		* @param dwReturnSize - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME virtual void asyncWriteFileComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** абсолютный путь до файла */
		std::filesystem::path _filePath;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END