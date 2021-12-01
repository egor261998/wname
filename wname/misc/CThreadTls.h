#pragma once

_WNAME_BEGIN
namespace misc
{
	/** TLS */
	template <class T>
	class CThreadTls
	{
	#pragma region Public_Method
	//==========================================================================
	public:
		/**
		* конструктор по умолчанию.
		*/
		WNAME CThreadTls() noexcept(false)
		{
			_dwTlsIndex = TlsAlloc();
			if (_dwTlsIndex == TLS_OUT_OF_INDEXES)
			{
				throw std::runtime_error("CThreadTls failed with error: " +
					std::to_string(GetLastError()));
			}
		}
	//==========================================================================
		/**
		* конструктор копирования.
		* @param tls - объект копирования.
		*/
		WNAME CThreadTls(
			const CThreadTls& tls) noexcept
		{
			*this = tls;
		}
	//==========================================================================
		/**
		* конструктор перемещения.
		* @param tls - объект перемещения.
		*/
		WNAME CThreadTls(
			CThreadTls&& tls) noexcept
		{
			*this = std::move(tls);
		}
	//==========================================================================
		/**
		* установить значение.
		* @param p - значение.
		* @return - результат установки.
		*/
		WNAME bool setValue(T* p) noexcept
		{
			return TlsSetValue(_dwTlsIndex, static_cast<PVOID>(p));
		}
	//==========================================================================
		/**
		* получить значение.
		* @return - значение.
		*/
		WNAME T* getValue() noexcept
		{
			return static_cast<T*>(TlsGetValue(_dwTlsIndex));
		}
	//==========================================================================
		/**
		* оператор копирования объекта.
		* @param tls - копируемый объект.
		* @return - текущий объект.
		*/
		WNAME CThreadTls& operator=(
			const CThreadTls& tls) noexcept
		{
			_dwTlsIndex = tls._dwTlsIndex;
		}
	//==========================================================================
		/**
		* оператор перемещения объекта.
		* @param tls - перемещаемый объект.
		* @return - текущий объект.
		*/
		WNAME CThreadTls& operator=(
			CThreadTls&& tls) noexcept
		{
			_dwTlsIndex = tls._dwTlsIndex;
			tls._dwTlsIndex = TLS_OUT_OF_INDEXES;
		}
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CThreadTls()
		{
			TlsFree(_dwTlsIndex);
		}
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** присвоенный индекс */
		DWORD _dwTlsIndex = TLS_OUT_OF_INDEXES;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END