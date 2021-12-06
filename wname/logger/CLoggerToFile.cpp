#include "../stdafx.h"

using CLoggerToFilePrefix = wname::logger::CLoggerToFile;

//==============================================================================
CLoggerToFilePrefix::CLoggerToFile(
	const std::filesystem::path logPath)
	:_logPath(logPath.lexically_normal())
{

}
//==============================================================================
void CLoggerToFilePrefix::logWrite(
	const std::wstring& wStr) const
{	
	std::wofstream fileStream(_logPath, std::ios::app);

	fileStream << wStr;
	fileStream.close();
}
//==============================================================================