#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** ��������� ��� ������ */
	class CSocketHandle : public handle::CHandle
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ������.
		*/
		WNAME CSocketHandle() noexcept = default;
	//==========================================================================
		/**
		* ����������� ������.
		* @patam type - ��� ������.
		* @param protocol - ��������.
		*/
		WNAME CSocketHandle(
			const int type,
			const int protocol);
	//==========================================================================
		/**
		* ����������� �����������.
		* @param socket - ��������� �����������.
		*/
		WNAME CSocketHandle(
			const CSocketHandle& socket) noexcept;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param socket - ��������� �����������.
		*/
		WNAME CSocketHandle(
			CSocketHandle&& socket) noexcept;
	//==========================================================================
		/**
		* �������� ������� AcceptEx.
		* @return - ��������� �� ������� AcceptEx.
		*/
		WNAME LPFN_ACCEPTEX getAcceptEx() const;
	//==========================================================================
		/**
		* �������� ������� ConnectEx.
		* @return - ��������� �� ������� ConnectEx.
		*/
		WNAME LPFN_CONNECTEX getConnectEx() const;
	//==========================================================================
		/**
		* �������� ������� GetAcceptExSockAddrs.
		* @return - ��������� �� ������� GetAcceptExSockAddrs.
		*/
		WNAME LPFN_GETACCEPTEXSOCKADDRS getGetAcceptExSockAddrs() const;
	//==========================================================================
		/**
		* �������� ������� Disconnectex.
		* @return - ��������� �� ������� Disconnectex.
		*/
		WNAME LPFN_DISCONNECTEX getDisconnectex() const;
	//==========================================================================
		/**
		* �������� ������ ������.
		* @return - ������ ������.
		*/
		WNAME SOCKET getSocket() const noexcept;
	//==========================================================================
		/**
		* �������� ���������� ������.
		* @param socket - ����� �����.
		* @return - ������� ������.
		*/
		WNAME CSocketHandle& operator=(
			const SOCKET socket);
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param socket - ���������� ������.
		* @return - ������� ������.
		*/
		WNAME CSocketHandle& operator=(
			const CSocketHandle& socket) noexcept;
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param socket - ������������ ������.
		* @return - ������� ������.
		*/
		WNAME CSocketHandle& operator=(
			CSocketHandle&& socket) noexcept;
	//==========================================================================
		/**
		* �������� ��������� ������.
		* @return - ���������.
		*/
		WNAME operator SOCKET() const noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		WNAME ~CSocketHandle();
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* ��������� WSAIoctl ��� ������.
		* @param guid - ������.
		* @return - ��������� �� �������.
		*/
		template<class T>
		WNAME T getWSAIoctl(GUID& guid) const;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END