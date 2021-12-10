#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** ����� ������ */
	class CSocketAddress
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		WNAME CSocketAddress() noexcept = default;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param socketAddress - ������ �����������.
		*/
		WNAME CSocketAddress(
			const CSocketAddress& socketAddress) noexcept;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param socketAddress - ������ �����������.
		*/
		WNAME CSocketAddress(
			CSocketAddress&& socketAddress) noexcept;
	//==========================================================================
		/**
		* ����������� ������.
		* @param strIp - IP �����
		* @param wPort - ����.
		*/
		WNAME CSocketAddress(
			const std::string strIp,
			const WORD wPort);
	//==========================================================================
		/**
		* ����������� ������.
		* @param pSockAdr - ����� ������.
		*/
		WNAME CSocketAddress(
			const sockaddr* const pSockAdr) noexcept;
	//==========================================================================
		/**
		* ����������� ������.
		* @param pSockAdr_in - ����� ������.
		*/
		WNAME CSocketAddress(
			const sockaddr_in* const pSockAdr_in) noexcept;
	//==========================================================================
		/**
		* �������� ������� ����� ��� xxx.xxx.xxx.xxx:xxxxx.
		* @return - ������� �����.
		*/
		WNAME std::wstring getAddress() const;
	//==========================================================================
	/**
		* �������� ������ ������.
		* @return - ������.
		*/
		WNAME constexpr DWORD size() const noexcept;
	//==========================================================================
		/**
		* ���������� ����������.
		*/
		WNAME operator sockaddr_in* () noexcept;
		WNAME operator sockaddr* () noexcept;
	//==========================================================================
		/**
		* �������� �����������.
		* @param socketAddress - ���������� ������.
		* @return - ������� ������.
		*/
		WNAME CSocketAddress& operator=(
			const CSocketAddress& socketAddress) noexcept;
	//==========================================================================
		/**
		* �������� �����������.
		* @param socketAddress - ������������ ������.
		* @return - ������� ������.
		*/
		WNAME CSocketAddress& operator=(
			CSocketAddress&& socketAddress) noexcept;
	//==========================================================================
		/**
		* ����������
		*/
		WNAME ~CSocketAddress();
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** �������������� ��������� ����� */
		union
		{
			sockaddr_in _bindEndpoint_in{ 0 };
			sockaddr _bindEndpoint;
		};
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END