#pragma once

_WNAME_BEGIN
namespace network
{
	/** ����� TCP ������� */
	class CTcpServer : protected socket::CSocketIo
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** ����� ������������� ������� � TCP ������� */
		class CTcpConnectedClient;
		friend class CTcpConnectedClient;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� TCP �������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		WNAME CTcpServer(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* �������� TCP ������.
		* @return - ��������� ����������.
		*/
		WNAME std::error_code connectServer();
	//==========================================================================
		/**
		* ��������� TCP ������.
		* @param ec - ��� ������ ����������.
		*/
		WNAME void disconnectServer(
			const std::error_code ec = std::error_code()) noexcept;
	//==========================================================================
		/**
		* ���������� TCP �������.
		*/
		WNAME ~CTcpServer();
	//==========================================================================
		CTcpServer(const CTcpServer&) = delete;
		CTcpServer(CTcpServer&&) = delete;
		CTcpServer& operator=(const CTcpServer&) = delete;
		CTcpServer& operator=(CTcpServer&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* �������� ������� �� ������.
		* @param pClient - ��������� ������.
		*/
		WNAME void removeClient(
			CTcpConnectedClient* const pClient) noexcept;
	//==========================================================================
		/**
		* �������� ������� � ������.
		* @return - ������������ ������.
		*/
		WNAME CTcpConnectedClient* addClient();
	//==========================================================================
		/**
		* ���������� ������� �������� ������ ����������� �� ���� TCP ��������.
		* @param pAsyncOperation - ����������� ��������.
		*/
		WNAME static void clientAcceptedEventHandler(
			io::iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* ����� ������������� ��� �������� ������ �����������.
		* @return - ��� ������.
		*/
		WNAME std::error_code startListen();
	//==========================================================================
		/**
		* ����������� ���������� �������� �������.
		* @return - ������
		*/
		WNAME virtual std::unique_ptr<CTcpConnectedClient> createClient();
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		WNAME virtual void serverConnected(
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		WNAME virtual void serverDisconnected(
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ��������� */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** ����� ������� */
		socket::CSocketAddress _socketAddress;
		/** ����� ������� */
		socket::CSocketHandle _socket;

		/** ������������ ������� */
		std::unordered_map<CTcpConnectedClient*, std::shared_ptr<CTcpConnectedClient>> _listClients;
	
		/** ������ */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};

	using CTcpConnectedClient = CTcpServer::CTcpConnectedClient;
}
_WNAME_END