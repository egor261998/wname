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
		void removeClient(
			CTcpConnectedClient* const pClient) noexcept;
	//==========================================================================
		/**
		* �������� ������� � ������.
		* @return - ������������ ������.
		*/
		CTcpConnectedClient* addClient();
	//==========================================================================
		/**
		* ���������� ������� �������� ������ ����������� �� ���� TCP ��������.
		* @param pAsyncOperation - ����������� ��������.
		*/
		static void clientAcceptedEventHandler(
			io::iocp::CAsyncOperation* const pAsyncOperation) noexcept;
	//==========================================================================
		/**
		* ����� ������������� ��� �������� ������ �����������.
		* @return - ��� ������.
		*/
		std::error_code startListen();
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ������������ ������.
		* @param pTcpClient - ������.
		* @param bufferRecv - ����� ������.
		* @param dwBufferSize - ���������� ����������� ����.
		* @param ec - ��� ����������.
		*/
		virtual void clientAsyncRecvComplite(
			CTcpConnectedClient* const pTcpClient,
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ����������� ������.
		* @param pTcpClient - ������.
		* @param bufferSend - ���������� ������������ ����.
		* @param dwBufferSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		virtual void clientAsyncSendComplite(
			CTcpConnectedClient* const pTcpClient,
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* ����������� ���������� ������� ����������� ������ �������.
		* @param pTcpClient - ������ �� �������� �������.
		* @param ec - ��� ������ ����������.
		*/
		virtual void clientConnected(
			CTcpConnectedClient* const pTcpClient,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� �������.
		* @param pTcpClient - ������.
		* @param ec - ��� ������ ����������.
		*/
		virtual void clientDisconnected(
			CTcpConnectedClient* const pTcpClient,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		virtual void serverConnected(
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		virtual void serverDisconnected(
			const std::error_code ec) noexcept = 0;
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
		std::unordered_map<CTcpConnectedClient*, std::unique_ptr<CTcpConnectedClient>> _listClients;
	
		/** ������ */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};

	using CTcpConnectedClient = CTcpServer::CTcpConnectedClient;
}
_WNAME_END