#include "../stdafx.h"

using wname::filesystem::CDirectory;

//==============================================================================
CDirectory::CDirectory(
	const std::filesystem::path directoryPath,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) : 
	CAsyncIo(pIocp),
	_pParent(nullptr)
{
	try
	{
		_directoryPath = directoryPath;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
CDirectory::CDirectory(
	const std::filesystem::path directoryPath,
	const std::shared_ptr<io::iocp::CIocp>& pIocp,
	CDirectory* const pParent) : 
	CAsyncIo(pIocp),
	_pParent(pParent)
{
	try
	{
		if (pParent == nullptr)
			throw std::invalid_argument("pParent == nullptr");

		_directoryPath = directoryPath;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CDirectory::createDirectory(
	const bool isSubOpenDirectory,
	const bool isNotify,
	const DWORD dwDesiredAccess,
	const DWORD dwShareMode,
	const DWORD dwCreationDisposition,
	const DWORD dwFlagsAndAttributes)
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	cs::CCriticalSectionScoped lock(_csCounter);

	_isSubOpenDirectory = isSubOpenDirectory;
	_isNotify = isNotify;

	try
	{
		const auto directoryPath = getPath();

		if (dwCreationDisposition == CREATE_NEW ||
			dwCreationDisposition == CREATE_ALWAYS)
		{
			/** на случай если папки нет */
			const auto directoryParentPath = directoryPath.parent_path();
			std::error_code ec;
			const auto bIsExists = std::filesystem::exists(directoryParentPath, ec);

			if (ec)
			{
				return ec;
			}

			if (!bIsExists)
			{
				std::filesystem::create_directories(directoryParentPath, ec);

				if (ec)
				{
					return ec;
				}
			}
		}

		handle::CHandle hHandle = CreateFile(
			directoryPath.c_str(),
			dwDesiredAccess,
			dwShareMode,
			NULL,
			dwCreationDisposition,
			dwFlagsAndAttributes | FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		if (!hHandle.isValid())
			return std::error_code(GetLastError(), std::system_category());

		/** инициализация асинхронных операций */
		bindHandle(hHandle);

		if (_isNotify; std::error_code ec = startNotify())
		{
			/** ошибка уведомлений */
			close();

			return ec;
		}

		if (std::error_code ec = enumDirectory())
		{
			/** ошибка получения вложений */
			close();

			return ec;
		}

		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		close();
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::filesystem::path CDirectory::getPath()
{
	cs::CCriticalSectionScoped lock(_csCounter);

	try
	{
		return _directoryPath;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
bool CDirectory::isOpen() noexcept
{
	return isBindHandle();
}
//==============================================================================
std::list<std::filesystem::path> CDirectory::getFileList(
	const bool bSubFile,
	const std::filesystem::path proximatePath)
{
	try
	{
		misc::CCounterScoped counter(*this);
		if (!counter.isStartOperation())
			throw std::logic_error("Directory is not open");

		cs::CCriticalSectionScoped lock(_csCounter);

		std::list<std::filesystem::path> listPath;

		for (const auto& [k, v] : _listFile)
		{
			listPath.push_back(v->getPath().lexically_proximate(proximatePath));
		}

		if (bSubFile)
		{
			for (const auto& [k, v] : _listDirectory)
			{
				auto subList = v->getFileList(bSubFile, proximatePath);
				std::move(subList.begin(), subList.end(), std::back_inserter(listPath));
			}
		}

		return listPath;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CDirectory::deleteDirectory(
	const std::filesystem::path directoryPath) noexcept
{
	const auto bResult = RemoveDirectory(directoryPath.c_str());

	return std::error_code(
		bResult ? ERROR_SUCCESS : GetLastError(),
		std::system_category());
}
//==============================================================================
std::error_code CDirectory::deleteDirectory()
{
	try
	{
		return deleteDirectory(getPath());
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CDirectory::close() noexcept
{
	release(true);

	cs::CCriticalSectionScoped lock(_csCounter);

	_listDirectory.clear();
	_listFile.clear();
}
//==============================================================================
std::error_code CDirectory::enumDirectory()
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	cs::CCriticalSectionScoped lock(_csCounter);

	try
	{
		WIN32_FIND_DATA ffd = {0};
		auto directoryPath = getPath().wstring();
		std::wstring pathFind = directoryPath;
		pathFind.append(L"\\*");
		auto hFind = FindFirstFile(pathFind.c_str(), &ffd);
		
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				try
				{		
					std::wstring path = directoryPath;
					path.append(L"\\");
					path.append(&ffd.cFileName[0]);

					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (_wcsicmp(&ffd.cFileName[0], L".") == 0 ||
							_wcsicmp(&ffd.cFileName[0], L"..") == 0)
						{
							/** эти элементы пропускаем,
								иначе отсканируем всю систему
								еще и упремся в рекурсию*/
							continue;
						}
						else if (ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
						{
							/** системная папка, не добавляем в список */
							continue;
						}
						else
						{
							/** нашлась папка */
							addDirectoryToDirectory(path);
						}
					}
					else
					{
						/** нашелся файл */
						addFileToDirectory(path);
					}
				}
				catch (const std::exception& ex)
				{
					_pIocp->log(logger::EMessageType::critical, ex);
					FindClose(hFind);
					throw;
				}

			} while (FindNextFile(hFind, &ffd));

			FindClose(hFind);
		}

		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::warning, ex);
		throw;
	}
}
//==============================================================================
void CDirectory::addFileToDirectory(
	const std::filesystem::path& filePath)
{
	cs::CCriticalSectionScoped lock(_csCounter);

	try
	{
		misc::CCounterScoped counter(*this);
		if (!counter.isStartOperation())
			throw std::logic_error("Directory is not open");

		BY_HANDLE_FILE_INFORMATION info = {};
		HANDLE hFile = CreateFile(
			filePath.c_str(), 
			0, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			throw std::runtime_error("hFile == INVALID_HANDLE_VALUE");

		if (!GetFileInformationByHandle(hFile, &info))
		{
			CloseHandle(hFile);
			throw std::runtime_error("GetFileInformationByHandle is Failed");
		}
		CloseHandle(hFile);

		LARGE_INTEGER li = {};
		li.LowPart = info.nFileIndexLow;
		li.HighPart = info.nFileIndexHigh;

		auto pFile = std::make_shared<CFile>(filePath, _pIocp);

		_listFile[li.QuadPart] = pFile;
		changeSubFileCount(1);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::warning, ex);
		throw;
	}
}
//==============================================================================
void CDirectory::addDirectoryToDirectory(
	const std::filesystem::path& directoryPath)
{
	cs::CCriticalSectionScoped lock(_csCounter);

	try
	{
		misc::CCounterScoped counter(*this);
		if (!counter.isStartOperation())
			throw std::logic_error("Directory is not open");

		BY_HANDLE_FILE_INFORMATION info = {};
		HANDLE hDirectory = CreateFile(
			directoryPath.c_str(), 
			0, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		if (hDirectory == INVALID_HANDLE_VALUE)
			throw std::runtime_error("hDirectory == INVALID_HANDLE_VALUE");

		if (!GetFileInformationByHandle(hDirectory, &info))
		{
			CloseHandle(hDirectory);
			throw std::runtime_error("GetFileInformationByHandle is Failed");
		}
		CloseHandle(hDirectory);

		LARGE_INTEGER li = {};
		li.LowPart = info.nFileIndexLow;
		li.HighPart = info.nFileIndexHigh;

		if (_listDirectory.find(li.QuadPart) != _listDirectory.end())
		{
			/** папка уже есть */
			return;
		}

		auto pDirectory = std::make_shared<CDirectory>(
			directoryPath, 
			_pIocp, 
			this);

		const auto ec = pDirectory->createDirectory(
			_isSubOpenDirectory, _isNotify,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_DELETE,
			OPEN_ALWAYS);

		if (ec)
		{
			throw std::runtime_error("CreateDirectory failed: " + ec.message());
		}

		_listDirectory[li.QuadPart] = std::move(pDirectory);
		
		changeSubDirectoryCount(1);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::warning, ex);
		throw;
	}
}
//==============================================================================
void CDirectory::removeFileFromDirectory(
	const UINT64 uId) noexcept
{
	#pragma warning(disable: 26472)
	cs::CCriticalSectionScoped lock(_csCounter);

	const auto count = static_cast<int>(_listFile.erase(uId));
	changeSubFileCount(-count);
}
//==============================================================================
void CDirectory::removeDirectoryFromDirectory(
	const UINT64 uId) noexcept
{
	#pragma warning(disable: 26472)
	cs::CCriticalSectionScoped lock(_csCounter);

	const auto count = static_cast<int>(_listDirectory.erase(uId));
	changeSubDirectoryCount(-count);
}
//==============================================================================
void CDirectory::changeSubFileCount(
	const int countItem) noexcept
{
	_nCountSubFile += countItem;
	if (_pParent != nullptr)
	{
		_pParent->changeSubFileCount(countItem);
	}
}
//==============================================================================
void CDirectory::changeSubDirectoryCount(
	const int countItem) noexcept
{
	_nCountSubDirectory += countItem;
	if (_pParent != nullptr)
	{
		_pParent->changeSubDirectoryCount(countItem);
	}
}
//==============================================================================
std::error_code CDirectory::startNotify()
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	cs::CCriticalSectionScoped lock(_csCounter);

	try
	{
		if (_bufferNotify.size() != BUFFER_64K)
		{
			_bufferNotify.resize(BUFFER_64K);
		}

		auto pAsyncOperation =
			_pIocp->getAsyncOperation(this, notifyCompilteHandler);
		assert(pAsyncOperation != nullptr);

		pAsyncOperation->_buffer._p = _bufferNotify.data();
		pAsyncOperation->_buffer._dwSize = static_cast<DWORD>(_bufferNotify.size());

		const auto bRes = ReadDirectoryChangesExW(
			_hAsyncHandle,
			pAsyncOperation->_buffer._p,
			pAsyncOperation->_buffer._dwSize, false,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_LAST_ACCESS |
			FILE_NOTIFY_CHANGE_CREATION |
			FILE_NOTIFY_CHANGE_SECURITY,
			&pAsyncOperation->_dwReturnSize,
			&pAsyncOperation->_overlapped,
			NULL,
			ReadDirectoryNotifyExtendedInformation);

		if (!bRes)
		{
			pAsyncOperation->cancel();
			return std::error_code(GetLastError(), std::system_category());
		}
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::warning, ex);
		throw;
	}

	counter.release();
	return std::error_code();
}
//==============================================================================
void CDirectory::notifyCompilteHandler(
	io::iocp::CAsyncOperation* const pAsyncOperation) noexcept
{
	#pragma warning (disable: 26446 26493)
	assert(pAsyncOperation != nullptr);

	assert(pAsyncOperation->_pCompletionRoutineContext != nullptr);
	const auto& _this = static_cast<CDirectory*>(
		pAsyncOperation->_pCompletionRoutineContext);

	const auto retryNotify = [](
		CDirectory* const pDirectory)
	{
		if (pDirectory == nullptr)
			throw std::invalid_argument("pDirectory == nullptr");

		cs::CCriticalSectionScoped lock(pDirectory->_csCounter);

		pDirectory->_listDirectory.clear();
		pDirectory->_listFile.clear();

		std::error_code ec;
		auto bIsRetryAgain = false;

		if (ec = pDirectory->startNotify(); ec)
		{
			/** ошибка уведомлений */
			pDirectory->_pIocp->log(
				logger::EMessageType::warning,
				L"Error start notify",
				ec);
			bIsRetryAgain = pDirectory->notifyErrorCompilteHandler(ec);
		}
		else if (ec = pDirectory->enumDirectory(); ec)
		{
			/** ошибка получения вложений */
			pDirectory->_pIocp->log(
				logger::EMessageType::warning,
				L"Error enum directory",
				ec);
			bIsRetryAgain = pDirectory->notifyErrorCompilteHandler(ec);
		}

		return bIsRetryAgain;
	};

	try
	{
		if (!pAsyncOperation->_ec && pAsyncOperation->_dwReturnSize != 0)
		{
			/** уведомление успешное, в обработку */
			const auto bufferNotifyInfo = std::vector<BYTE>(
				&_this->_bufferNotify[0],
				&_this->_bufferNotify[pAsyncOperation->_dwReturnSize]);

			auto pNotifyInfo =
				(PFILE_NOTIFY_EXTENDED_INFORMATION)bufferNotifyInfo.data();

			_this->notifyAcceptCompilteHandler(pNotifyInfo);
			_this->endOperation();
			return;
		}

		if (pAsyncOperation->_ec.value() == ERROR_OPERATION_ABORTED)
		{
			/** прервано */
			_this->endOperation();
			return;
		}

		if (pAsyncOperation->_ec)
		{
			_this->_pIocp->log(
				logger::EMessageType::warning,
				L"Notify error",
				pAsyncOperation->_ec);

			if (_this->notifyErrorCompilteHandler(pAsyncOperation->_ec))
			{
				/** пробуем перезапускаться */
				while (retryNotify(_this));
			}

			/** ошибка */
			_this->endOperation();
			return;
		}

		/** кончился буфер для уведомления,
			необходимо пере открыть папку */
		while (retryNotify(_this));
	}
	catch (const std::exception& ex)
	{
		_this->_pIocp->log(logger::EMessageType::warning, ex);

		if (_this->notifyErrorCompilteHandler(
			std::error_code(ERROR_FUNCTION_FAILED,
				std::system_category())))
		{
			auto bIsRetry = true;
		
			/** пробуем перезапускаться */
			while (bIsRetry)
			{
				try
				{
					bIsRetry = retryNotify(_this);
				}
				catch (const std::exception& exRetry)
				{
					_this->_pIocp->log(logger::EMessageType::warning, exRetry);
				}
			}				
		}		
	}
	
	_this->endOperation();
}
//==============================================================================
void CDirectory::notifyAcceptCompilteHandler(
	PFILE_NOTIFY_EXTENDED_INFORMATION pNotifyInfo)
{
	#pragma warning (disable: 26493 26482 26446)
	assert(pNotifyInfo != nullptr);

	cs::CCriticalSectionScoped lock(_csCounter);

	try
	{

		if (const auto ec = startNotify(); ec)
			throw std::runtime_error("Start Notify Failed: " + ec.message());

		do
		{

			const std::wstring pathViaDirectory(
				&pNotifyInfo->FileName[0],
				&pNotifyInfo->FileName[pNotifyInfo->FileNameLength / sizeof(WCHAR)]);

			std::filesystem::path path = _directoryPath;
			path.append(L"\\");
			path.append(pathViaDirectory);
			path = path.lexically_normal();

			assert(pNotifyInfo->Action != 0);
			switch (pNotifyInfo->Action)
			{
			case FILE_ACTION_ADDED:
			{
				assert(pNotifyInfo->FileId.QuadPart != 0);
				if (pNotifyInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					addDirectoryToDirectory(path);
					notifyDirectoryCompilteHandler(
						pNotifyInfo->Action,
						_listDirectory.at(pNotifyInfo->FileId.QuadPart));
				}
				else
				{
					addFileToDirectory(path);
					notifyFileCompilteHandler(
						pNotifyInfo->Action,
						_listFile.at(pNotifyInfo->FileId.QuadPart));
				}

				break;
			}
			case FILE_ACTION_REMOVED:
			{
				if (pNotifyInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					notifyDirectoryCompilteHandler(
						pNotifyInfo->Action,
						_listDirectory.at(pNotifyInfo->FileId.QuadPart));
					removeDirectoryFromDirectory(pNotifyInfo->FileId.QuadPart);
				}
				else
				{
					notifyFileCompilteHandler(
						pNotifyInfo->Action,
						_listFile.at(pNotifyInfo->FileId.QuadPart));
					removeFileFromDirectory(pNotifyInfo->FileId.QuadPart);
				}

				break;
			}
			case FILE_ACTION_RENAMED_NEW_NAME:
			{
				if (pNotifyInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					auto pDirectory = _listDirectory.at(pNotifyInfo->FileId.QuadPart);
					{
						cs::CCriticalSectionScoped lockDirectory(pDirectory->_csCounter);
						pDirectory->_directoryPath = path;
					}
					notifyDirectoryCompilteHandler(
						pNotifyInfo->Action,
						pDirectory);
				}
				else
				{
					auto pFile = _listFile.at(pNotifyInfo->FileId.QuadPart);
					{
						cs::CCriticalSectionScoped lockFile(pFile->_csCounter);
						pFile->_filePath = path;
					}
					notifyFileCompilteHandler(
						pNotifyInfo->Action,
						pFile);
				}

				break;
			}
			default:
			{
				if (pNotifyInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					notifyDirectoryCompilteHandler(
						pNotifyInfo->Action,
						_listDirectory.at(pNotifyInfo->FileId.QuadPart));
				}
				else
				{
					notifyFileCompilteHandler(
						pNotifyInfo->Action,
						_listFile.at(pNotifyInfo->FileId.QuadPart));
				}

				break;
			}
			}

			if (pNotifyInfo->NextEntryOffset == 0)
				break;

			pNotifyInfo = PFILE_NOTIFY_EXTENDED_INFORMATION(
				(PBYTE)pNotifyInfo + pNotifyInfo->NextEntryOffset);
		} while (true);

	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CDirectory::notifyFileCompilteHandler(
	const DWORD dwAction,
	const std::shared_ptr<CFile> pFile,
	const bool bIsSubItem)
{
	UNREFERENCED_PARAMETER(bIsSubItem);

	if (_pParent)
	{
		_pParent->notifyFileCompilteHandler(dwAction, pFile, true);
	}

	/** пропускаем */
}
//==============================================================================
void CDirectory::notifyDirectoryCompilteHandler(
	const DWORD dwAction,
	const std::shared_ptr<CDirectory> pDirectory,
	const bool bIsSubItem)
{
	UNREFERENCED_PARAMETER(bIsSubItem);

	if (_pParent)
	{
		_pParent->notifyDirectoryCompilteHandler(dwAction, pDirectory, true);
	}

	/** пропускаем */
}
//==============================================================================
bool CDirectory::notifyErrorCompilteHandler(
	const std::error_code ec,
	const bool bIsSubItem) noexcept
{
	UNREFERENCED_PARAMETER(bIsSubItem);

	if (_pParent)
	{
		return _pParent->notifyErrorCompilteHandler(ec, true);
	}

	/** пропускаем */
	return false;
}
//==============================================================================
CDirectory::~CDirectory()
{
	release(true);
}
//==============================================================================