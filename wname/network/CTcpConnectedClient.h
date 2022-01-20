#pragma once

_WNAME_BEGIN
namespace network
{
	/** ����� ������������� ������� � TCP ������� */
	class WNAME CTcpServer::CTcpConnectedClient : public ITcpClient
	{
		friend class CTcpServer;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ������������� �������.
		* @param pParent - ����������� ����� TCP �������.
		*/
		CTcpConnectedClient(
			CTcpServer* const pParent);
	//==========================================================================
		/**
		* ���������� ����������.
		* @return - ��� ������.
		*/
		std::error_code connect() noexcept override;
	//==========================================================================
		/**
		* �������� ����� ������ �������.
		* @return - ����� ������ �������.
		*/
		socket::CSocketAddress getAddress() noexcept override;
	//==========================================================================
		/**
		* �������� ����� ������ ���������� �������.
		* @return - ����� ������ �������.
		*/
		socket::CSocketAddress getAddressRemote() noexcept override;
	//==========================================================================
		/**
		* ��������� ����������.
		* @param ec - ��� ������.
		*/
		void disconnect(
			const std::error_code ec = std::error_code()) noexcept override;
	//==========================================================================
		/**
		* ��������� ������.
		* @param bIsWait - ������� ��������.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* ����������.
		*/
		~CTcpConnectedClient();
	//==========================================================================
		CTcpConnectedClient(const CTcpConnectedClient&) = delete;
		CTcpConnectedClient(CTcpConnectedClient&&) = delete;
		CTcpConnectedClient& operator=(const CTcpConnectedClient&) = delete;
		CTcpConnectedClient& operator=(CTcpConnectedClient&&) = delete;
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
		socket::CSocketAddress _localAddress;
		/** ��������� ����� */
		socket::CSocketAddress _remotelAddress;

		/** ����� ��� ����������� */
		BYTE _bufferConnect[(sizeof(sockaddr) + 16) * 2]{ 0 };
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END