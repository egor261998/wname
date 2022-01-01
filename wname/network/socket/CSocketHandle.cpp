#include "../../stdafx.h"

using CSocketHandlePrefix = wname::network::socket::CSocketHandle;

//==============================================================================
CSocketHandlePrefix::CSocketHandle(
	const int type,
	const int protocol)
{
	/** создаем сокет */
	*this = ::socket(AF_INET, type, protocol);
}
//==============================================================================
CSocketHandlePrefix::CSocketHandle(
	const CSocketHandle& socket) noexcept
{
	/** выполняем команду через перегрузку оператора копирования */
	*this = socket;
}
//==============================================================================
CSocketHandlePrefix::CSocketHandle(
	CSocketHandle&& socket) noexcept
{
	/** выполняем команду через перегрузку оператора перемещения */
	*this = std::move(socket);
}
//==============================================================================
LPFN_ACCEPTEX CSocketHandlePrefix::getAcceptEx() const
{
	GUID guid = WSAID_ACCEPTEX;
	return getWSAIoctl<LPFN_ACCEPTEX>(guid);
}
//==============================================================================
LPFN_CONNECTEX CSocketHandlePrefix::getConnectEx() const
{
	GUID guid = WSAID_CONNECTEX;
	return getWSAIoctl<LPFN_CONNECTEX>(guid);
}
//==============================================================================
LPFN_GETACCEPTEXSOCKADDRS CSocketHandlePrefix::getGetAcceptExSockAddrs() const
{
	GUID guid = WSAID_GETACCEPTEXSOCKADDRS;
	return getWSAIoctl<LPFN_GETACCEPTEXSOCKADDRS>(guid);
}
//==============================================================================
LPFN_DISCONNECTEX CSocketHandlePrefix::getDisconnectex() const
{
	GUID guid = WSAID_DISCONNECTEX;
	return getWSAIoctl<LPFN_DISCONNECTEX>(guid);
}
//==============================================================================
template<class T>
T CSocketHandlePrefix::getWSAIoctl(GUID& guid) const
{
	T p = NULL;
	DWORD dwBytes = 0;

	if (WSAIoctl(
		getSocket(),
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid),
		&p, sizeof(p),
		&dwBytes, NULL, NULL) == SOCKET_ERROR)
	{
		throw std::runtime_error("WSAIoctl failed with error: " +
			std::to_string(WSAGetLastError()));
	}

	return p;
}
//==============================================================================
SOCKET CSocketHandlePrefix::getSocket() const noexcept
{
	#pragma warning (disable: 26493)
	return (SOCKET)getHandle();
}
//==============================================================================
bool CSocketHandlePrefix::setKeepAlive(
	const bool bValue) const noexcept
{
	const int yes = bValue ? 1 : 0;

	return setsockopt(getSocket(), SOL_SOCKET, SO_KEEPALIVE, (char*)&yes, sizeof(yes)) != SOCKET_ERROR;
}
//==============================================================================
CSocketHandlePrefix::operator SOCKET() const noexcept
{
	return getSocket();
}
//==============================================================================
CSocketHandlePrefix& CSocketHandlePrefix::operator=(
	const SOCKET socket)
{
	#pragma warning (disable: 26493)

	/** закрываем предыдущий описатель */
	close();

	if (isValid((HANDLE)socket))
	{
		/** описатель валидный */
		_pHandle = std::shared_ptr<void>(
			(HANDLE)socket,
			[](HANDLE s) noexcept {closesocket((SOCKET)s); });
		return *this;
	}

	return *this;
}
//==============================================================================
CSocketHandlePrefix& CSocketHandlePrefix::operator=(
	const CSocketHandle& handle) noexcept
{
	CHandle::operator=(handle);
	return *this;;
}
//==============================================================================
CSocketHandlePrefix& CSocketHandlePrefix::operator=(
	CSocketHandle&& handle) noexcept
{
	CHandle::operator=(std::move(handle));
	return *this;
}
//==============================================================================
CSocketHandlePrefix::~CSocketHandle()
{
	close();
}
//==============================================================================
