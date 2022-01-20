#include "../stdafx.h"

using wname::logger::CLoggerToFile;

//==============================================================================
CLoggerToFile::CLoggerToFile(
	const std::filesystem::path logPath) :
	_logPath(logPath.lexically_normal())
{

}
//==============================================================================
void CLoggerToFile::logWrite(
	const std::wstring& wStr) const
{	
	std::wofstream fileStream(_logPath, std::ios::app);

	fileStream << wStr;
	fileStream.close();
}
//==============================================================================