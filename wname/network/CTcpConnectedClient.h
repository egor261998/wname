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
		* ����� ����������� ������ � �����.
		* @param bufferSend - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		WNAME std::error_code startAsyncSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize,
			const DWORD dwFlags) override;
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
			const DWORD dwFlags) override;
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
			const DWORD dwFlags) override;
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
			const DWORD dwFlags) override;
	//==========================================================================
		/**
		* ��������� ����������.
		* @param ec - ��� ������.
		*/
		WNAME void disconnect(
			const std::error_code ec = std::error_code()) noexcept;
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

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������.
		* @param bufferSend - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ������ ����������.
		*/
		void asyncSendComplitionHandler(
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
		void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ��������� */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** ��������� ����� */
		socket::CSocketAddress _localAddres;
		/** ��������� ����� */
		socket::CSocketAddress _remotelAddres;
		/** ����� ������� */
		socket::CSocketHandle _socket;

		/** ������ */
		std::error_code _ec;

		/** �������� ������� */
		PVOID _pContex = nullptr;

		/** ��������� �� ����������� ����� */
		CTcpServer* const _pParent;

		/** ����� ��� ����������� */
		BYTE _bufferConnect[(sizeof(sockaddr) + 16) * 2]{ 0 };
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END