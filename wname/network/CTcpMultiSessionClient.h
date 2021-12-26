#pragma once

_WNAME_BEGIN
namespace network
{
	/** ������ ����� ����������� �������� ������ */
	class CTcpMiltiSessionClient : protected ITcpMultiSessionProtocol
	{
	#pragma region Private_Inner
	private:
	//==========================================================================
		class CTcpClientPart : public wname::network::CTcpClient
		{
			friend class CTcpMiltiSessionClient;
		#pragma region Public_Method
		public:
		//======================================================================
			/**
			* ����������� TCP ������.
			* @param strIp - IP ����� �������.
			* @param wPort - ���� �������.
			* @param pIocp - �������� Iocp.
			* @param pCTcpMiltiSessionClient - ����������� �����.
			*/
			WNAME CTcpClientPart(
				const std::string& strIp,
				const WORD wPort,
				const std::shared_ptr<io::iocp::CIocp>& pIocp,
				CTcpMiltiSessionClient* const pCTcpMiltiSessionClient)
				: CTcpClient(strIp, wPort, pIocp),
				_pCTcpMiltiSessionClient(pCTcpMiltiSessionClient)
			{
				try
				{
					if (_pCTcpMiltiSessionClient == nullptr)
						throw std::invalid_argument("_pCTcpMiltiSessionClient == nullptr");

					if(CoCreateGuid(&_guid) != S_OK)
						throw std::runtime_error("CoCreateGuid is failed");

					/** ��������� ������ �� ����������� ������ */
					if(!_pCTcpMiltiSessionClient->startOperation())
						throw std::runtime_error("_pCTcpMiltiSessionClient is not start");
				}
				catch (const std::exception& ex)
				{
					pIocp->log(logger::EMessageType::critical, ex);
					throw;
				}
			}
		//======================================================================
			/**
			* ���������� ������� ���������� ������������ ������.
			* @param bufferRecv - ����� ������.
			* @param dwReturnSize - ���������� ���������� ����.
			* @param ec - ��� ����������.
			*/
			WNAME void clientAsyncRecvComplitionHandler(
				const PBYTE bufferRecv,
				const DWORD dwReturnSize,
				const std::error_code ec) noexcept override
			{
				_pCTcpMiltiSessionClient->clientAsyncRecvComplitionHandler(
					this, bufferRecv, dwReturnSize, ec);
			}
		//======================================================================
			/**
			* ���������� ������� ���������� ����������� ������.
			* @param bufferSend - ����� ������.
			* @param dwReturnSize - ���������� ���������� ����.
			* @param ec - ��� ����������.
			*/
			WNAME void clientAsyncSendComplitionHandler(
				const PBYTE bufferSend,
				const DWORD dwReturnSize,
				const std::error_code ec) noexcept override
			{
				_pCTcpMiltiSessionClient->clientAsyncSendComplitionHandler(
					this, bufferSend, dwReturnSize, ec);
			}
		//======================================================================
			/**
			* ���������� ������� ����������� �������.
			* @param ec - ��� ������.
			*/
			WNAME void clientConnected(
				const std::error_code ec) noexcept override
			{
				_pCTcpMiltiSessionClient->clientConnected(
					this, ec);
			}
		//======================================================================
			/**
			* ���������� ������� ���������� �������.
			* @param ec - ��� ������.
			*/
			WNAME void clientDisconnected(
				const std::error_code ec) noexcept override
			{
				_pCTcpMiltiSessionClient->clientDisconnected(
					this, ec);
			}
		//======================================================================
			/**
			* ����������.
			*/
			WNAME ~CTcpClientPart()
			{
				/** ��������� */
				disconnect();

				/** ���� ���������� ����� */
				release();

				/** ������� ������ � ������������ ������� */
				_pCTcpMiltiSessionClient->endOperation();
			}
		//======================================================================
		#pragma endregion

		#pragma region Public_Data
		public:
		//======================================================================
			/** ����������� */
			GUID _guid;
		//======================================================================
		#pragma endregion

		#pragma region Private_Data
		private:
		//======================================================================
			/** ����������� ����� */
			CTcpMiltiSessionClient* const _pCTcpMiltiSessionClient;
		//======================================================================
		#pragma endregion
		};
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ����� ����������� �������� ������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		WNAME CTcpMiltiSessionClient(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ���������� ����������.
		* @return - ��� ������.
		*/
		WNAME std::error_code connect() noexcept override;
	//==========================================================================
		/**
		* ����� ����������� ������.
		* @param bufferSend - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @return - ��� ������.
		*/
		WNAME std::error_code startAsyncSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize) override;
	//==========================================================================
		/**
		* ����� ������������ ������.
		* @param bufferRecv - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @return - ��� ������.
		*/
		WNAME std::error_code startAsyncRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize) override;
	//==========================================================================
		/**
		* ��������� ����������.
		* @param ec - ��� ������ ����������.
		*/
		WNAME void disconnect(
			const std::error_code ec = std::error_code()) noexcept override;
	//==========================================================================
		/**
		* ����������.
		*/
		WNAME ~CTcpMiltiSessionClient();
	//==========================================================================
		CTcpMiltiSessionClient(const CTcpMiltiSessionClient&) = delete;
		CTcpMiltiSessionClient(CTcpMiltiSessionClient&&) = delete;
		CTcpMiltiSessionClient& operator=(const CTcpMiltiSessionClient&) = delete;
		CTcpMiltiSessionClient& operator=(CTcpMiltiSessionClient&&) = delete;
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
		/**
		* ���������� ������� ����������� �������.
		* @param pTcpClientPart - ������.
		* @param ec - ��� ������.
		*/
		WNAME void clientConnected(
			CTcpClientPart* const pTcpClientPart,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������ ������ �������.
		* @param pTcpClientPart - ������.
		* @param bufferRecv - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		WNAME void clientAsyncRecvComplitionHandler(
			CTcpClientPart* const pTcpClientPart,
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������ ������ �������.
		* @param pTcpClientPart - ������.
		* @param bufferSend - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		WNAME void clientAsyncSendComplitionHandler(
			CTcpClientPart* const pTcpClientPart,
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ���������� ������� ���������� ������ �������.
		* @param pTcpClientPart - ������.
		* @param ec - ��� ������.
		*/
		WNAME void clientDisconnected(
			CTcpClientPart* const pTcpClientPart,
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ����������� ������ */
		std::unique_ptr<CTcpClientPart> _pTcpClientPartControl;

		/** ������ ��� �������� */
		std::unordered_map<GUID, std::unique_ptr<CTcpClientPart>> _transferSession;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END