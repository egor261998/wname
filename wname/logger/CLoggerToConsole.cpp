#include "../stdafx.h"

using CLoggerToConsolePrefix = wname::logger::CLoggerToConsole;

//==============================================================================
CLoggerToConsolePrefix::CLoggerToConsole() noexcept
{

}
//==============================================================================
void CLoggerToConsolePrefix::logWrite(
	const std::wstring& wStr) const noexcept
{
	wprintf(L"\n%s", wStr.c_str());
}
//==============================================================================