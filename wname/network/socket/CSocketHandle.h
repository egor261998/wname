#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** ��������� ��� ������ */
	class WNAME CSocketHandle final : public handle::CHandle
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ������.
		*/
		CSocketHandle() noexcept = default;
	//==========================================================================
		/**
		* ����������� ������.
		* @patam type - ��� ������.
		* @param protocol - ��������.
		*/
		CSocketHandle(
			const int type,
			const int protocol);
	//==========================================================================
		/**
		* ����������� �����������.
		* @param socket - ��������� �����������.
		*/
		CSocketHandle(
			const CSocketHandle& socket) noexcept;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param socket - ��������� �����������.
		*/
		CSocketHandle(
			CSocketHandle&& socket) noexcept;
	//==========================================================================
		/**
		* �������� ������� AcceptEx.
		* @return - ��������� �� ������� AcceptEx.
		*/
		LPFN_ACCEPTEX getAcceptEx() const;
	//==========================================================================
		/**
		* �������� ������� ConnectEx.
		* @return - ��������� �� ������� ConnectEx.
		*/
		LPFN_CONNECTEX getConnectEx() const;
	//==========================================================================
		/**
		* �������� ������� GetAcceptExSockAddrs.
		* @return - ��������� �� ������� GetAcceptExSockAddrs.
		*/
		LPFN_GETACCEPTEXSOCKADDRS getGetAcceptExSockAddrs() const;
	//==========================================================================
		/**
		* �������� ������� Disconnectex.
		* @return - ��������� �� ������� Disconnectex.
		*/
		LPFN_DISCONNECTEX getDisconnectex() const;
	//==========================================================================
		/**
		* �������� ������ ������.
		* @return - ������ ������.
		*/
		SOCKET getSocket() const noexcept;
	//==========================================================================
		/**
		* ���������� �������� keep alive ��� ������� ������.
		* @param bValue - �������� ���������.
		* @return - ��� ������.
		*/
		std::error_code setKeepAlive(
			const bool bValue) const noexcept;
	//==========================================================================
		/**
		* �������� ���������� ������.
		* @param socket - ����� �����.
		* @return - ������� ������.
		*/
		CSocketHandle& operator=(
			const SOCKET socket);
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param socket - ���������� ������.
		* @return - ������� ������.
		*/
		CSocketHandle& operator=(
			const CSocketHandle& socket) noexcept;
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param socket - ������������ ������.
		* @return - ������� ������.
		*/
		CSocketHandle& operator=(
			CSocketHandle&& socket) noexcept;
	//==========================================================================
		/**
		* �������� ��������� ������.
		* @return - ���������.
		*/
		operator SOCKET() const noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		~CSocketHandle();
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
		T getWSAIoctl(GUID& guid) const;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END