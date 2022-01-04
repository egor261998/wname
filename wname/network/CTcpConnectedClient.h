#pragma once

_WNAME_BEGIN
namespace network
{
	/** ����� ������������� ������� � TCP ������� */
	class CTcpServer::CTcpConnectedClient : protected CSocketIo
	{
		friend class CTcpServer;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ������������� �������.
		* @param pParent - ����������� ����� TCP �������.
		*/
		WNAME CTcpConnectedClient(
			CTcpServer* const pParent);
	//==========================================================================
			/**
			* ���������� �������� keep alive ��� �������.
			* @param bValue - �������� ���������.
			* @return - ��� ������.
			*/
		WNAME std::error_code setKeepAlive(
			const bool bValue) noexcept;
	//==========================================================================
		/**
		* �������� ����� ������ �������.
		* @return - ����� ������ �������.
		*/
		WNAME socket::CSocketAddress getClientAddress() noexcept;
		//==========================================================================
		/**
		* �������� ����� ������ �������.
		* @return - ����� ������ �������.
		*/
		WNAME socket::CSocketAddress getServerAddress() noexcept;
	//==========================================================================
		/**
		* �������� ��������� ������.
		* @return - ��������� ������.
		*/
		WNAME socket::ESocketState getState() noexcept;
	//==========================================================================
		/**
		* ����� ����������� ������ � �����.
		* @param bufferSend - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		WNAME std::error_code startAsyncSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* ����� ���������� ������ � �����.
		* @param bufferSend - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @param pdwReturnSize - ���������� ���������� ����.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		WNAME std::error_code startSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* ����� ����������� ������ �� ������.
		* @param bufferRecv - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		WNAME std::error_code startAsyncRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* ����� ����������� ������ �� ������.
		* @param bufferRecv - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @param pdwReturnSize - ���������� ���������� ����.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		WNAME std::error_code startRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* ��������� ����������.
		* @param ec - ��� ������.
		*/
		WNAME void disconnect(
			const std::error_code ec = std::error_code()) noexcept;
	//==========================================================================
		/**
		* ��������� ������.
		*/
		WNAME void release() noexcept override;
	//==========================================================================
		/**
		* ����������.
		*/
		WNAME ~CTcpConnectedClient();
	//==========================================================================
		CTcpConnectedClient(const CTcpConnectedClient&) = delete;
		CTcpConnectedClient(CTcpConnectedClient&&) = delete;
		CTcpConnectedClient& operator=(const CTcpConnectedClient&) = delete;
		CTcpConnectedClient& operator=(CTcpConnectedClient&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ������������ ������.
		* @param bufferRecv - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		WNAME virtual void clientAsyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ����������� ������.
		* @param bufferSend - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		WNAME virtual void clientAsyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ����������� �������.
		* @param ec - ��� ������.
		*/
		WNAME virtual void clientConnected(
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		WNAME virtual void clientDisconnected(
			const std::error_code ec) noexcept;	
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* �������� ����������� �������� � ���������.
		*/
		WNAME void addAsyncIoPending() noexcept;
	//==========================================================================
		/**
		* ������� ����������� �������� �� ���������.
		*/
		WNAME void delAsyncIoPending() noexcept;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������.
		* @param bufferSend - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ������ ����������.
		*/
		WNAME void asyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������.
		* @param bufferRecv - ����� ������.
		* @param dwReturnSize - ���������� ����������� ����.
		* @param ec - ��� ������ ����������.
		*/
		WNAME void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ��������� �� ����������� ����� */
		CTcpServer* const _pParent;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ��������� */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** ��������� ����� */
		socket::CSocketAddress _localAddress;
		/** ��������� ����� */
		socket::CSocketAddress _remotelAddress;
		/** ����� ������� */
		socket::CSocketHandle _socket;

		/** ���������� ����������� �������� � ��������� */
		size_t _nAsyncIoPending = 0;

		/** ������ */
		std::error_code _ec;

		/** ����� ��� ����������� */
		BYTE _bufferConnect[(sizeof(sockaddr) + 16) * 2]{ 0 };
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END