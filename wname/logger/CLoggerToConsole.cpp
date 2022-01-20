#include "../stdafx.h"

using wname::logger::CLoggerToConsole;

//==============================================================================
void CLoggerToConsole::logWrite(
	const std::wstring& wStr) const noexcept
{
	wprintf(L"\n%s", wStr.c_str());
}
//==============================================================================