#pragma once

_WNAME_BEGIN
namespace logger
{
	/** типы сообщения */
	enum class EMessageType : UINT8
	{
		trace,
		warning,
		critical
	};
}
_WNAME_END