#pragma once

_WNAME_BEGIN
namespace network
{
	/** ����� TCP ������� */
	class WNAME CTcpClient : public ITcpClient
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� TCP �������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		CTcpClient(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ���������� ����������.
		* @return - ��� ������.
		*/
		std::error_code connect() noexcept override;
	//==========================================================================
		/**
		* �������� ����� ������.
		* @return - ����� ������.
		*/
		socket::CSocketAddress getAddress() noexcept override;
	//==========================================================================
		/**
		* �������� ����� ������ ���������� �������.
		* @return - ����� ������.
		*/
		socket::CSocketAddress getAddressRemote() noexcept override;
	//==========================================================================
		/**
		* ��������� ����������.
		* @param ec - ��� ������ ����������.
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
		~CTcpClient();
	//==========================================================================
		CTcpClient(const CTcpClient&) = delete;
		CTcpClient(CTcpClient&&) = delete;
		CTcpClient& operator=(const CTcpClient&) = delete;
		CTcpClient& operator=(CTcpClient&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ����� ������� ����������� */
		socket::CSocketAddress _socketAddress;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END