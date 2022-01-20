#include "../../stdafx.h"

using wname::network::socket::CSocketHandle;

//==============================================================================
CSocketHandle::CSocketHandle(
	const int type,
	const int protocol)
{
	/** создаем сокет */
	*this = ::socket(AF_INET, type, protocol);
}
//==============================================================================
CSocketHandle::CSocketHandle(
	const CSocketHandle& socket) noexcept
{
	/** выполняем команду через перегрузку оператора копирования */
	*this = socket;
}
//==============================================================================
CSocketHandle::CSocketHandle(
	CSocketHandle&& socket) noexcept
{
	/** выполняем команду через перегрузку оператора перемещения */
	*this = std::move(socket);
}
//==============================================================================
LPFN_ACCEPTEX CSocketHandle::getAcceptEx() const
{
	GUID guid = WSAID_ACCEPTEX;
	return getWSAIoctl<LPFN_ACCEPTEX>(guid);
}
//==============================================================================
LPFN_CONNECTEX CSocketHandle::getConnectEx() const
{
	GUID guid = WSAID_CONNECTEX;
	return getWSAIoctl<LPFN_CONNECTEX>(guid);
}
//==============================================================================
LPFN_GETACCEPTEXSOCKADDRS CSocketHandle::getGetAcceptExSockAddrs() const
{
	GUID guid = WSAID_GETACCEPTEXSOCKADDRS;
	return getWSAIoctl<LPFN_GETACCEPTEXSOCKADDRS>(guid);
}
//==============================================================================
LPFN_DISCONNECTEX CSocketHandle::getDisconnectex() const
{
	GUID guid = WSAID_DISCONNECTEX;
	return getWSAIoctl<LPFN_DISCONNECTEX>(guid);
}
//==============================================================================
template<class T>
T CSocketHandle::getWSAIoctl(GUID& guid) const
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
SOCKET CSocketHandle::getSocket() const noexcept
{
	#pragma warning (disable: 26493)
	return (SOCKET)getHandle();
}
//==============================================================================
std::error_code CSocketHandle::setKeepAlive(
	const bool bValue) const noexcept
{
	const int yes = bValue ? 1 : 0;

	if (setsockopt(
		getSocket(),
		SOL_SOCKET,
		SO_KEEPALIVE,
		(char*)&yes, sizeof(yes)) != SOCKET_ERROR)
	{
		return std::error_code(WSAGetLastError(), std::system_category());
	}

	return std::error_code();
}
//==============================================================================
CSocketHandle::operator SOCKET() const noexcept
{
	return getSocket();
}
//==============================================================================
CSocketHandle& CSocketHandle::operator=(
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
CSocketHandle& CSocketHandle::operator=(
	const CSocketHandle& handle) noexcept
{
	CHandle::operator=(handle);
	return *this;;
}
//==============================================================================
CSocketHandle& CSocketHandle::operator=(
	CSocketHandle&& handle) noexcept
{
	CHandle::operator=(std::move(handle));
	return *this;
}
//==============================================================================
CSocketHandle::~CSocketHandle()
{
	close();
}
//==============================================================================
