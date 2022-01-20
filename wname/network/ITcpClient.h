#pragma once

_WNAME_BEGIN
namespace network
{
	/** ��������� TCP ������� */
	class WNAME ITcpClient : protected socket::CSocketIo
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� TCP ����������.
		* @param pIocp - �������� Iocp.
		*/
		ITcpClient(
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ����������� ���������� ����������.
		* @return - ��� ������.
		*/
		virtual std::error_code connect() noexcept = 0;
	//==========================================================================
		/**
		* ���������� �������� keep alive ��� �������.
		* @param bValue - �������� ���������.
		* @return - ��� ������.
		*/
		std::error_code setKeepAlive(
			const bool bValue) noexcept;
	//==========================================================================
		/**
		* ����� ����������� ������ � �����.
		* @param bufferSend - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		std::error_code startAsyncSend(
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
		std::error_code startSend(
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
		std::error_code startAsyncRecv(
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
		std::error_code startRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u) override;
	//==========================================================================
		/**
		* �������� ����� ������ �������.
		* @return - ����� ������.
		*/
		virtual socket::CSocketAddress getAddress() noexcept = 0;
	//==========================================================================
		/**
		* �������� ����� ������ ���������� �������.
		* @return - ����� ������.
		*/
		virtual socket::CSocketAddress getAddressRemote() noexcept = 0;
	//==========================================================================
		/**
		* �������� ��������� ������.
		* @return - ��������� ������.
		*/
		socket::ESocketState getState() noexcept;
	//==========================================================================
		/**
		* ���������� ��������� ����������.
		* @param ec - ��� ������ ����������.
		*/
		virtual void disconnect(
			const std::error_code ec = std::error_code()) noexcept = 0;
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
		~ITcpClient();
	//==========================================================================
		ITcpClient(const ITcpClient&) = delete;
		ITcpClient(ITcpClient&&) = delete;
		ITcpClient& operator=(const ITcpClient&) = delete;
		ITcpClient& operator=(ITcpClient&&) = delete;
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
		virtual void clientAsyncRecvComplitionHandler(
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
		virtual void clientAsyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ����������� �������.
		* @param ec - ��� ������.
		*/
		virtual void clientConnected(
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		virtual void clientDisconnected(
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* �������� ����������� �������� � ���������.
		*/
		void addAsyncIoPending() noexcept;
	//==========================================================================
		/**
		* ������� ����������� �������� �� ���������.
		*/
		void delAsyncIoPending() noexcept;
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

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ��������� */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** ����� ������� */
		socket::CSocketHandle _socket;

		/** ���������� ����������� �������� � ��������� */
		size_t _nAsyncIoPending = 0;

		/** ������ */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END