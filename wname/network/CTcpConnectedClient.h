#pragma once

_WNAME_BEGIN
namespace network
{
	/** класс подключенного клиента к TCP серверу */
	class WNAME CTcpServer::CTcpConnectedClient : public ITcpClient
	{
		friend class CTcpServer;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор подключаемого клиента.
		* @param pParent - родительный класс TCP сервера.
		*/
		CTcpConnectedClient(
			CTcpServer* const pParent);
	//==========================================================================
		/**
		* синхронное соединение.
		* @return - код ошибки.
		*/
		std::error_code connect() noexcept override;
	//==========================================================================
		/**
		* получить адрес сокета клиента.
		* @return - адрес сокета клиента.
		*/
		socket::CSocketAddress getAddress() noexcept override;
	//==========================================================================
		/**
		* получить адрес сокета удаленного клиента.
		* @return - адрес сокета сервера.
		*/
		socket::CSocketAddress getAddressRemote() noexcept override;
	//==========================================================================
		/**
		* разорвать соединение.
		* @param ec - код ошибки.
		*/
		void disconnect(
			const std::error_code ec = std::error_code()) noexcept override;
	//==========================================================================
		/**
		* закончить работу.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* деструктор.
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
		/** указатель на родительный класс */
		CTcpServer* const _pParent;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		socket::CSocketAddress _localAddress;
		/** удаленный адрес */
		socket::CSocketAddress _remotelAddress;

		/** буфер для подключения */
		BYTE _bufferConnect[(sizeof(sockaddr) + 16) * 2]{ 0 };
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END