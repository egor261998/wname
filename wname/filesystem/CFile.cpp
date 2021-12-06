#include "../stdafx.h"

using CFilePrefix = wname::filesystem::CFile;

//==============================================================================
CFilePrefix::CFile(
	const std::filesystem::path filePath,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) 
:CAsyncIo(pIocp)
{
	try
	{
		_filePath = filePath;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CFilePrefix::createFile(
	const DWORD dwDesiredAccess,
	const DWORD dwShareMode,
	const DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes)
{
	cs::CCriticalSectionScoped lock(_csCounter);

	dwFlagsAndAttributes |= FILE_FLAG_OVERLAPPED;

	if (isInitialize())
		return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());

	try
	{
		const auto filePath = getPath();

		if (dwCreationDisposition == CREATE_NEW ||
			dwCreationDisposition == CREATE_ALWAYS)
		{
			/** на случай если папки нет */
			const auto directoryPath = filePath.parent_path();
			std::error_code ec;
			const auto bIsExists = std::filesystem::exists(directoryPath, ec);

			if (ec)
			{
				return ec;
			}

			if (!bIsExists)
			{
				std::filesystem::create_directories(directoryPath, ec);

				if (ec)
				{
					return ec;
				}
			}
		}
			
		handle::CHandle hHandle = CreateFile(
			filePath.c_str(),
			dwDesiredAccess,
			dwShareMode,
			NULL,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			NULL);

		if (!hHandle.isValid())
			return std::error_code(GetLastError(), std::system_category());

		/** инициализация асинхронных операций */
		initialize(hHandle);

		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CFilePrefix::startAsyncReadFile(
	const PBYTE bufferRead,
	const DWORD dwBufferSize,
	const UINT64 offset)
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		const auto ec = startAsyncRead(
			bufferRead,
			dwBufferSize,
			offset);

		if (!ec)
			counter.release();

		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CFilePrefix::startReadFile(
	const PBYTE bufferRead,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	const UINT64 offset)
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		return startRead(
			bufferRead,
			dwBufferSize,
			pdwReturnSize,
			offset);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CFilePrefix::startAsyncWriteFile(
	const PBYTE bufferWrite,
	const DWORD dwBufferSize,
	const UINT64 offset)
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());
		
	try
	{
		const auto ec = startAsyncWrite(
			bufferWrite,
			dwBufferSize,
			offset);

		if (!ec)
			counter.release();

		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CFilePrefix::startWriteFile(
	const PBYTE bufferWrite,
	const DWORD dwBufferSize,
	const PDWORD pdwReturnSize,
	const UINT64 offset)
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	try
	{
		return startWrite(
			bufferWrite,
			dwBufferSize,
			pdwReturnSize,
			offset);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CFilePrefix::close() noexcept
{
	release();
}
//==============================================================================
bool CFilePrefix::isOpen() noexcept
{
	return isInitialize();
}
//==============================================================================
std::error_code CFilePrefix::getFileSize(
	UINT64& uSize)
{
	std::error_code ec;
	uSize = std::filesystem::file_size(getPath(), ec);

	return ec;
}
//==============================================================================
std::error_code CFilePrefix::deleteFile()
{
	return deleteFile(getPath());
}
//==============================================================================
std::error_code CFilePrefix::clearFile() noexcept
{
	DWORD dwResult = ERROR_SUCCESS;
	const auto hHandle = getHandle();

	if (SetFilePointer(hHandle, 0, NULL, FILE_BEGIN) != 0)
	{
		dwResult = GetLastError();
	}
	else
	{
		if (!SetEndOfFile(hHandle))
			dwResult = GetLastError();
	}

	return std::error_code(dwResult, std::system_category());
}
//==============================================================================
std::filesystem::path CFilePrefix::getPath()
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _filePath;
}
//==============================================================================
std::error_code CFilePrefix::deleteFile(
	const std::filesystem::path filePath) noexcept
{
	const auto bResult = DeleteFile(filePath.c_str());

	return std::error_code(
		bResult ? ERROR_SUCCESS : GetLastError(), 
		std::system_category());
}
//==============================================================================
void CFilePrefix::asyncReadComplitionHandler(
	const PBYTE bufferRead,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	asyncReadFileComplitionHandler(bufferRead, dwReturnSize, ec);

	endOperation();
};
//==============================================================================
void CFilePrefix::asyncWriteComplitionHandler(
	const PBYTE bufferWrite,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	asyncWriteFileComplitionHandler(bufferWrite, dwReturnSize, ec);

	endOperation();
};
//==============================================================================
void CFilePrefix::asyncReadFileComplitionHandler(
	const PBYTE bufferRead,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferRead);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
};
//==============================================================================
void CFilePrefix::asyncWriteFileComplitionHandler(
	const PBYTE bufferWrite,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferWrite);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
};
//==============================================================================
CFilePrefix::~CFile()
{
	close();
}
//==============================================================================