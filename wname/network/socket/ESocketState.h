#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** ��������� ������ */
	enum class ESocketState
	{
		disconnected,
		disconnecting,

		connecting,
		connected,
	};
}
_WNAME_END