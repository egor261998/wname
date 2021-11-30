#include "../stdafx.h"

using IloggerPrefix = wname::logger::ILogger;
using EMessageTypePrefix = wname::logger::EMessageType;

/** определяем количество попыток логирования */
std::atomic_long IloggerPrefix::_uRepeatLog = 3;

//==============================================================================
void IloggerPrefix::log(
	const EMessageTypePrefix eMessageType,
	const std::exception& ex,
	const std::error_code ec,
	const char* const szMessageFunction) const noexcept
{
	try
	{
		log(eMessageType, ex.what(), ec, szMessageFunction);
	}
	catch (const std::exception& exWhat)
	{
		log(eMessageType, exWhat, ec, szMessageFunction);
	}	
}
//==============================================================================
void IloggerPrefix::log(
	const EMessageTypePrefix eMessageType,
	const char* const szMessage,
	const std::error_code ec,
	const char* const szMessageFunction) const noexcept
{
	for (long i = 0; i < _uRepeatLog; i++)
	{
		try
		{
			std::string strMessage(szMessage != nullptr ? szMessage : "UnknownMessage");
			log(eMessageType, strMessage, ec, szMessageFunction);

			/** логируем дальше */
			break;
		}
		catch (const std::exception& ex)
		{
			UNREFERENCED_PARAMETER(ex);

			/** если возникает исключение при логирование, то просто пропускаем */
			continue;
		}
	}
}
//==============================================================================
void IloggerPrefix::log(
	const EMessageTypePrefix eMessageType,
	const std::string strMessage,
	const std::error_code ec,
	const char* const szMessageFunction) const noexcept
{
	for (long i = 0; i < _uRepeatLog; i++)
	{
		try
		{
			std::wstring wStrMessage(strMessage.begin(), strMessage.end());
			log(eMessageType, wStrMessage, ec, szMessageFunction);

			/** логируем дальше */
			break;
		}
		catch (const std::exception& ex)
		{
			UNREFERENCED_PARAMETER(ex);

			/** если возникает исключение при логирование, то просто пропускаем */
			continue;
		}
	}
}
//==============================================================================
void IloggerPrefix::log(
	const EMessageTypePrefix eMessageType,
	const wchar_t* const wszMessage,
	const std::error_code ec,
	const char* const szMessageFunction) const noexcept
{
	for (long i = 0; i < _uRepeatLog; i++)
	{
		try
		{
			std::wstring wStrMessage(wszMessage != nullptr ? wszMessage : L"UnknownMessage");
			log(eMessageType, wStrMessage, ec, szMessageFunction);

			/** логируем дальше */
			break;
		}
		catch (const std::exception& ex)
		{
			UNREFERENCED_PARAMETER(ex);

			/** если возникает исключение при логирование, то просто пропускаем */
			continue;
		}
	}
}
//==============================================================================
void IloggerPrefix::log(
	const EMessageTypePrefix eMessageType,
	const std::wstring wStrMessage,
	const std::error_code ec,
	const char* const szMessageFunction) const noexcept
{
	/** конечная функция логирования */
	for (long i = 0; i < _uRepeatLog; i++)
	{
		try
		{
			std::string strMessageFunction(szMessageFunction != nullptr ? szMessageFunction : "UnknownFunction");
			std::wstring wStrMessageFunction(strMessageFunction.begin(), strMessageFunction.end());

			/** сборка сообщения */
			auto wStr = assembleMessage(eMessageType, wStrMessage, ec, wStrMessageFunction);

			/** логирование */
			logWrite(wStr);

			/** логирование успешно */
			break;
		}
		catch (const std::exception& ex)
		{
			UNREFERENCED_PARAMETER(ex);

			/** если возникает исключение при логирование, то просто пропускаем */
			continue;
		}
	}
}
//==============================================================================
std::wstring IloggerPrefix::assembleMessage(
	const EMessageType eMessageType,
	const std::wstring& wStrMessage,
	const std::error_code& ec,
	const std::wstring& wStrFunction) const
{
	std::wstring wStr = L"\n\nType: ";

	switch (eMessageType)
	{
	case EMessageType::trace:
		wStr.append(L"trace");
		break;
	case EMessageType::warning:
		wStr.append(L"warning");
		break;
	case EMessageType::critical:
		wStr.append(L"critical");
		break;
	default:
		wStr.append(L"Unknown");
		break;
	}

	wStr.append(L"\nMessage: " + wStrMessage);
	wStr.append(L"\nError code: " + std::to_wstring(ec.value()));
	wStr.append(L"\nMessage Function: " + wStrFunction);

	return wStr;
}
//==============================================================================