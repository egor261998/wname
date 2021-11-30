#pragma once

_WNAME_BEGIN
namespace logger
{
	/** типы сообщения */
	enum class EMessageType : int
	{
		trace,
		warning,
		critical
	};
}
_WNAME_END