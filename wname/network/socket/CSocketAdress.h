#pragma once

_WNAME_BEGIN
namespace network::socket
{
	/** адрес сокета */
	class WNAME CSocketAddress final
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		*/
		CSocketAddress() noexcept = default;
	//==========================================================================
		/**
		* конструктор копирования.
		* @param socketAddress - объект копирования.
		*/
		CSocketAddress(
			const CSocketAddress& socketAddress) noexcept;
	//==========================================================================
		/**
		* конструктор перемещения.
		* @param socketAddress - объект перемещения.
		*/
		CSocketAddress(
			CSocketAddress&& socketAddress) noexcept;
	//==========================================================================
		/**
		* конструктор адреса.
		* @param strIp - IP адрес
		* @param wPort - порт.
		*/
		CSocketAddress(
			const std::string strIp,
			const WORD wPort);
	//==========================================================================
		/**
		* конструктор адреса.
		* @param pSockAdr - адрес сокета.
		*/
		CSocketAddress(
			const sockaddr* const pSockAdr) noexcept;
	//==========================================================================
		/**
		* конструктор адреса.
		* @param pSockAdr_in - адрес сокета.
		*/
		CSocketAddress(
			const sockaddr_in* const pSockAdr_in) noexcept;
	//==========================================================================
		/**
		* получить сетевой адрес как xxx.xxx.xxx.xxx:xxxxx.
		* @return - сетевой адрес.
		*/
		std::wstring getAddress() const;
	//==========================================================================
	/**
		* получить размер адреса.
		* @return - размер.
		*/
		DWORD size() const noexcept;
	//==========================================================================
		/**
		* перегрузка операторов.
		*/
		operator sockaddr_in* () noexcept;
		operator sockaddr* () noexcept;
	//==========================================================================
		/**
		* оператор копирования.
		* @param socketAddress - копируемый объект.
		* @return - текущий объект.
		*/
		CSocketAddress& operator=(
			const CSocketAddress& socketAddress) noexcept;
	//==========================================================================
		/**
		* оператор перемещения.
		* @param socketAddress - перемещаемый объект.
		* @return - текущий объект.
		*/
		CSocketAddress& operator=(
			CSocketAddress&& socketAddress) noexcept;
	//==========================================================================
		/**
		* деструктор
		*/
		~CSocketAddress();
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