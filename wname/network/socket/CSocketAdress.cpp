#include "../../stdafx.h"

using CSocketAddressPrefix = wname::network::socket::CSocketAddress;

//==========================================================================
CSocketAddressPrefix::CSocketAddress(
	const CSocketAddress& socketAddress) noexcept
{
	*this = socketAddress;
}
//==========================================================================
CSocketAddressPrefix::CSocketAddress(
	CSocketAddress&& socketAddress) noexcept
{
	*this = std::move(socketAddress);
}
//==============================================================================
CSocketAddressPrefix::CSocketAddress(
	const std::string strIp,
	const WORD wPort)
{
	#pragma warning (disable: 26493)
	DWORD dwAddress = 0;

	/** проверяем наличие IP адреса */
	if (strIp.length())
	{
		hostent* pHostInfo = nullptr;
		in_addr HostAddress = { 0 };

		if (isalpha((int)strIp.at(0)))
		{
			pHostInfo = gethostbyname(strIp.c_str());
		}
		else
		{
			HostAddress.S_un.S_addr = inet_addr(strIp.c_str());
			if (HostAddress.S_un.S_addr == INADDR_NONE)
			{
				throw std::logic_error("Error IP");
			}

			pHostInfo =
				gethostbyaddr((CHAR*)&HostAddress, 4, AF_INET);
		}

		if (!pHostInfo || pHostInfo->h_addr_list[0] == NULL)
		{
			throw std::logic_error("Host not find");
		}

		dwAddress = *(DWORD*)pHostInfo->h_addr_list[0];
	}
	else
		dwAddress = 0;

	/** информация об оконечной точки */
	_bindEndpoint_in.sin_family = AF_INET;
	_bindEndpoint_in.sin_addr.s_addr = dwAddress;
	_bindEndpoint_in.sin_port = htons(wPort);
}
//==============================================================================
CSocketAddressPrefix::CSocketAddress(
	const sockaddr* const pSockAdr) noexcept
{
	if (pSockAdr == nullptr)
		return;

	memcpy(&_bindEndpoint, pSockAdr, sizeof(pSockAdr));
}
//==============================================================================
CSocketAddressPrefix::CSocketAddress(
	const sockaddr_in* const pSockAdr_in) noexcept
{
	if (pSockAdr_in == nullptr)
		return;

	memcpy(&_bindEndpoint, pSockAdr_in, sizeof(pSockAdr_in));
}
//==============================================================================
std::wstring CSocketAddressPrefix::getAddress() const
{
	const auto wPort = htons(_bindEndpoint_in.sin_port);
	std::string strIp = inet_ntoa(_bindEndpoint_in.sin_addr);
	std::wstring wstrIp(strIp.begin(), strIp.end());
	wstrIp.append(L":");
	wstrIp.append(std::to_wstring(wPort));
	return wstrIp;
}
//==============================================================================
constexpr DWORD CSocketAddressPrefix::size() const noexcept
{
	return sizeof(sockaddr_in);
}
//==============================================================================
CSocketAddressPrefix::operator sockaddr_in* () noexcept
{
	return &_bindEndpoint_in;
}
//==============================================================================
CSocketAddressPrefix::operator sockaddr* () noexcept
{
	return &_bindEndpoint;
}
//==============================================================================
CSocketAddressPrefix& CSocketAddressPrefix::operator=(
	const CSocketAddress& socketAddress) noexcept
{
	memcpy(&_bindEndpoint, &socketAddress._bindEndpoint, sizeof(_bindEndpoint));
	return *this;
}
//==============================================================================
CSocketAddressPrefix& CSocketAddressPrefix::operator=(
	CSocketAddress&& socketAddress) noexcept
{
	memcpy(&_bindEndpoint, &socketAddress._bindEndpoint, sizeof(_bindEndpoint));
	memset(&socketAddress._bindEndpoint, 0, sizeof(socketAddress._bindEndpoint));
	return *this;
}
//==============================================================================
CSocketAddressPrefix::~CSocketAddress()
{
	memset(&_bindEndpoint, 0, sizeof(_bindEndpoint));
}
//==============================================================================
