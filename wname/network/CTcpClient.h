#pragma once

_WNAME_BEGIN
namespace network
{
	/** класс TCP клиента */
	class WNAME CTcpClient : public ITcpClient
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор TCP клиента.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		CTcpClient(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* синхронное соединение.
		* @return - код ошибки.
		*/
		std::error_code connect() noexcept override;
	//==========================================================================
		/**
		* получить адрес сокета.
		* @return - адрес сокета.
		*/
		socket::CSocketAddress getAddress() noexcept override;
	//==========================================================================
		/**
		* получить адрес сокета удаленного клиента.
		* @return - адрес сокета.
		*/
		socket::CSocketAddress getAddressRemote() noexcept override;
	//==========================================================================
		/**
		* разорвать соединение.
		* @param ec - код ошибки завершения.
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
		/** состояние */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** адрес сервера */
		socket::CSocketAddress _socketAddress;
		/** сокет клиента */
		socket::CSocketHandle _socket;

		/** количество асинхронных операций в обработке */
		size_t _nAsyncIoPending = 0;

		/** ошибка */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END