#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** ����� ������ */
	class WNAME CSocketAddress final
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		CSocketAddress() noexcept = default;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param socketAddress - ������ �����������.
		*/
		CSocketAddress(
			const CSocketAddress& socketAddress) noexcept;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param socketAddress - ������ �����������.
		*/
		CSocketAddress(
			CSocketAddress&& socketAddress) noexcept;
	//==========================================================================
		/**
		* ����������� ������.
		* @param strIp - IP �����
		* @param wPort - ����.
		*/
		CSocketAddress(
			const std::string strIp,
			const WORD wPort);
	//==========================================================================
		/**
		* ����������� ������.
		* @param pSockAdr - ����� ������.
		*/
		CSocketAddress(
			const sockaddr* const pSockAdr) noexcept;
	//==========================================================================
		/**
		* ����������� ������.
		* @param pSockAdr_in - ����� ������.
		*/
		CSocketAddress(
			const sockaddr_in* const pSockAdr_in) noexcept;
	//==========================================================================
		/**
		* �������� ������� ����� ��� xxx.xxx.xxx.xxx:xxxxx.
		* @return - ������� �����.
		*/
		std::wstring getAddress() const;
	//==========================================================================
	/**
		* �������� ������ ������.
		* @return - ������.
		*/
		DWORD size() const noexcept;
	//==========================================================================
		/**
		* ���������� ����������.
		*/
		operator sockaddr_in* () noexcept;
		operator sockaddr* () noexcept;
	//==========================================================================
		/**
		* �������� �����������.
		* @param socketAddress - ���������� ������.
		* @return - ������� ������.
		*/
		CSocketAddress& operator=(
			const CSocketAddress& socketAddress) noexcept;
	//==========================================================================
		/**
		* �������� �����������.
		* @param socketAddress - ������������ ������.
		* @return - ������� ������.
		*/
		CSocketAddress& operator=(
			CSocketAddress&& socketAddress) noexcept;
	//==========================================================================
		/**
		* ����������
		*/
		~CSocketAddress();
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