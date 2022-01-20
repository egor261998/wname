#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** описатель для сокета */
	class WNAME CSocketHandle final : public handle::CHandle
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор сокета.
		*/
		CSocketHandle() noexcept = default;
	//==========================================================================
		/**
		* конструктор сокета.
		* @patam type - тип сокета.
		* @param protocol - протокол.
		*/
		CSocketHandle(
			const int type,
			const int protocol);
	//==========================================================================
		/**
		* конструктор копирования.
		* @param socket - описатель копирования.
		*/
		CSocketHandle(
			const CSocketHandle& socket) noexcept;
	//==========================================================================
		/**
		* конструктор перемещения.
		* @param socket - описатель перемещения.
		*/
		CSocketHandle(
			CSocketHandle&& socket) noexcept;
	//==========================================================================
		/**
		* получить функцию AcceptEx.
		* @return - указатель на функцию AcceptEx.
		*/
		LPFN_ACCEPTEX getAcceptEx() const;
	//==========================================================================
		/**
		* получить функцию ConnectEx.
		* @return - указатель на функцию ConnectEx.
		*/
		LPFN_CONNECTEX getConnectEx() const;
	//==========================================================================
		/**
		* получить функцию GetAcceptExSockAddrs.
		* @return - указатель на функцию GetAcceptExSockAddrs.
		*/
		LPFN_GETACCEPTEXSOCKADDRS getGetAcceptExSockAddrs() const;
	//==========================================================================
		/**
		* получить функцию Disconnectex.
		* @return - указатель на функцию Disconnectex.
		*/
		LPFN_DISCONNECTEX getDisconnectex() const;
	//==========================================================================
		/**
		* получить объект сокета.
		* @return - объект сокета.
		*/
		SOCKET getSocket() const noexcept;
	//==========================================================================
		/**
		* установить значение keep alive для объекта сокета.
		* @param bValue - значение установки.
		* @return - код ошибки.
		*/
		std::error_code setKeepAlive(
			const bool bValue) const noexcept;
	//==========================================================================
		/**
		* оператор присвоения сокета.
		* @param socket - новый сокет.
		* @return - текущий объект.
		*/
		CSocketHandle& operator=(
			const SOCKET socket);
	//==========================================================================
		/**
		* оператор копирования описателя.
		* @param socket - копируемый объект.
		* @return - текущий объект.
		*/
		CSocketHandle& operator=(
			const CSocketHandle& socket) noexcept;
	//==========================================================================
		/**
		* оператор перемещения описателя.
		* @param socket - перемещаемый объект.
		* @return - текущий объект.
		*/
		CSocketHandle& operator=(
			CSocketHandle&& socket) noexcept;
	//==========================================================================
		/**
		* оператор получения сокета.
		* @return - описатель.
		*/
		operator SOCKET() const noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		~CSocketHandle();
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* выполнить WSAIoctl для сокета.
		* @param guid - запрос.
		* @return - указатель на функцию.
		*/
		template<class T>
		T getWSAIoctl(GUID& guid) const;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END