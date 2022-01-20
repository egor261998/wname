#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** �������� �����/������ � ������� */
	class WNAME CSocketIo : protected io::CAsyncIo
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		* @param pIocp - �������� �����/������.
		*/
		CSocketIo(
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ����� ������������ ������.
		* @param bufferRecv - ����� ��� ������.
		* @param bufferSize - ������ ������ ��� ������.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		virtual std::error_code startAsyncRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const DWORD dwFlags = 0u);
	//==========================================================================
		/**
		* ����� ������.
		* @param bufferRecv - ����� ��� ������.
		* @param bufferSize - ������ ������ ��� ������.
		* @param pdwReturnSize - ���������� ����������� ����.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		virtual std::error_code startRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u);
	//==========================================================================
		/**
		* ����� ����������� ������.
		* @param bufferSend - ����� ��� ��������.
		* @param bufferSize - ������ ������ ��� ������.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		virtual std::error_code startAsyncSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize,
			const DWORD dwFlags = 0u);
	//==========================================================================
		/**
		* ����� ������.
		* @param bufferSend - ����� ��� ��������.
		* @param bufferSize - ������ ������ ��� ������.
		* @param pdwReturnSize - ���������� ���������� ����.
		* @param dwFlags - �����.
		* @return - ��� ������.
		*/
		virtual std::error_code startSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize,
			const PDWORD pdwReturnSize,
			const DWORD dwFlags = 0u);
	//==========================================================================
		/**
		* ��������� ������ � ��������� ���� ����������� ��������.
		* @param bIsWait - ������� ��������.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* ����������.
		*/
		~CSocketIo();
	//==========================================================================
		CSocketIo(const CSocketIo&) = delete;
		CSocketIo(CSocketIo&&) = delete;
		CSocketIo& operator=(const CSocketIo&) = delete;
		CSocketIo& operator=(CSocketIo&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ������������ ������.
		* @param bufferRecv - ����� ������.
		* @param dwReturnSize - ���������� ����������� ����.
		* @param ec - ��� ������ ����������.
		*/
		virtual void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ����������� ������.
		* @param bufferSend - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ������ ����������.
		*/
		virtual void asyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������.
		* @param bufferRead - ����� ������.
		* @param dwReturnSize - ���������� ����������� ����.
		* @param ec - ��� ������ ����������.
		*/
		void asyncReadComplitionHandler(
			const PBYTE bufferRead,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������.
		* @param bufferWrite - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ������ ����������.
		*/
		void asyncWriteComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END