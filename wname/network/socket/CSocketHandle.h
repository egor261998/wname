#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** описатель для сокета */
	class CSocketHandle : public handle::CHandle
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор сокета.
		*/
		WNAME CSocketHandle() noexcept = default;
	//==========================================================================
		/**
		* конструктор сокета.
		* @patam type - тип сокета.
		* @param protocol - протокол.
		*/
		WNAME CSocketHandle(
			const int type,
			const int protocol);
	//==========================================================================
		/**
		* конструктор копирования.
		* @param socket - описатель копирования.
		*/
		WNAME CSocketHandle(
			const CSocketHandle& socket) noexcept;
	//==========================================================================
		/**
		* конструктор перемещения.
		* @param socket - описатель перемещения.
		*/
		WNAME CSocketHandle(
			CSocketHandle&& socket) noexcept;
	//==========================================================================
		/**
		* получить функцию AcceptEx.
		* @return - указатель на функцию AcceptEx.
		*/
		WNAME LPFN_ACCEPTEX getAcceptEx() const;
	//==========================================================================
		/**
		* получить функцию ConnectEx.
		* @return - указатель на функцию ConnectEx.
		*/
		WNAME LPFN_CONNECTEX getConnectEx() const;
	//==========================================================================
		/**
		* получить функцию GetAcceptExSockAddrs.
		* @return - указатель на функцию GetAcceptExSockAddrs.
		*/
		WNAME LPFN_GETACCEPTEXSOCKADDRS getGetAcceptExSockAddrs() const;
	//==========================================================================
		/**
		* получить функцию Disconnectex.
		* @return - указатель на функцию Disconnectex.
		*/
		WNAME LPFN_DISCONNECTEX getDisconnectex() const;
	//==========================================================================
		/**
		* получить объект сокета.
		* @return - объект сокета.
		*/
		WNAME SOCKET getSocket() const noexcept;
	//==========================================================================
		/**
		* оператор присвоения сокета.
		* @param socket - новый сокет.
		* @return - текущий объект.
		*/
		WNAME CSocketHandle& operator=(
			const SOCKET socket);
	//==========================================================================
		/**
		* оператор копирования описателя.
		* @param socket - копируемый объект.
		* @return - текущий объект.
		*/
		WNAME CSocketHandle& operator=(
			const CSocketHandle& socket) noexcept;
	//==========================================================================
		/**
		* оператор перемещения описателя.
		* @param socket - перемещаемый объект.
		* @return - текущий объект.
		*/
		WNAME CSocketHandle& operator=(
			CSocketHandle&& socket) noexcept;
	//==========================================================================
		/**
		* оператор получения сокета.
		* @return - описатель.
		*/
		WNAME operator SOCKET() const noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CSocketHandle();
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
		WNAME T getWSAIoctl(GUID& guid) const;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END