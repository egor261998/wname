#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** адрес сокета */
	class CSocketAddress
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		*/
		WNAME CSocketAddress() noexcept = default;
	//==========================================================================
		/**
		* конструктор копирования.
		* @param socketAddress - объект копирования.
		*/
		WNAME CSocketAddress(
			const CSocketAddress& socketAddress) noexcept;
	//==========================================================================
		/**
		* конструктор перемещения.
		* @param socketAddress - объект перемещения.
		*/
		WNAME CSocketAddress(
			CSocketAddress&& socketAddress) noexcept;
	//==========================================================================
		/**
		* конструктор адреса.
		* @param strIp - IP адрес
		* @param wPort - порт.
		*/
		WNAME CSocketAddress(
			const std::string strIp,
			const WORD wPort);
	//==========================================================================
		/**
		* конструктор адреса.
		* @param pSockAdr - адрес сокета.
		*/
		WNAME CSocketAddress(
			const sockaddr* const pSockAdr) noexcept;
	//==========================================================================
		/**
		* конструктор адреса.
		* @param pSockAdr_in - адрес сокета.
		*/
		WNAME CSocketAddress(
			const sockaddr_in* const pSockAdr_in) noexcept;
	//==========================================================================
		/**
		* получить сетевой адрес как xxx.xxx.xxx.xxx:xxxxx.
		* @return - сетевой адрес.
		*/
		WNAME std::wstring getAddress() const;
	//==========================================================================
	/**
		* получить размер адреса.
		* @return - размер.
		*/
		WNAME constexpr DWORD size() const noexcept;
	//==========================================================================
		/**
		* перегрузка операторов.
		*/
		WNAME operator sockaddr_in* () noexcept;
		WNAME operator sockaddr* () noexcept;
	//==========================================================================
		/**
		* оператор копирования.
		* @param socketAddress - копируемый объект.
		* @return - текущий объект.
		*/
		WNAME CSocketAddress& operator=(
			const CSocketAddress& socketAddress) noexcept;
	//==========================================================================
		/**
		* оператор перемещения.
		* @param socketAddress - перемещаемый объект.
		* @return - текущий объект.
		*/
		WNAME CSocketAddress& operator=(
			CSocketAddress&& socketAddress) noexcept;
	//==========================================================================
		/**
		* деструктор
		*/
		WNAME ~CSocketAddress();
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** сформированная оконечная точка */
		union
		{
			sockaddr_in _bindEndpoint_in{ 0 };
			sockaddr _bindEndpoint;
		};
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END