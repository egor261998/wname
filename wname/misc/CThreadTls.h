#pragma once

_WNAME_BEGIN
namespace misc
{
	/** TLS */
	template <class T>
	class WNAME CThreadTls final
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		*/
		CThreadTls() noexcept(false)
		{		
			/** выделяем новый индекс */
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
		CThreadTls(
			const CThreadTls& tls) noexcept
		{
			/** переходим к оператору копирования */
			*this = tls;
		}
	//==========================================================================
		/**
		* конструктор перемещения.
		* @param tls - объект перемещения.
		*/
		CThreadTls(
			CThreadTls&& tls) noexcept
		{
			/** переходим к оператору перемещения */
			*this = std::move(tls);
		}
	//==========================================================================
		/**
		* установить значение.
		* @param p - значение.
		* @return - результат установки.
		*/
		bool setValue(T* p) noexcept
		{
			return TlsSetValue(_dwTlsIndex, static_cast<PVOID>(p));
		}
	//==========================================================================
		/**
		* получить значение.
		* @return - значение.
		*/
		T* getValue() const noexcept
		{
			return static_cast<T*>(TlsGetValue(_dwTlsIndex));
		}
	//==========================================================================
		/**
		* оператор копирования объекта.
		* @param tls - копируемый объект.
		* @return - текущий объект.
		*/
		CThreadTls& operator=(
			const CThreadTls& tls) noexcept
		{
			_dwTlsIndex = tls._dwTlsIndex;
			return *this;
		}
	//==========================================================================
		/**
		* оператор перемещения объекта.
		* @param tls - перемещаемый объект.
		* @return - текущий объект.
		*/
		CThreadTls& operator=(
			CThreadTls&& tls) noexcept
		{
			_dwTlsIndex = tls._dwTlsIndex;
			tls._dwTlsIndex = TLS_OUT_OF_INDEXES;
			return *this;
		}
	//==========================================================================
		/**
		* деструктор.
		*/
		~CThreadTls()
		{
			if (_dwTlsIndex == TLS_OUT_OF_INDEXES)
			{
				/** индекса нет, высвобождать нечего */
				return;
			}

			TlsFree(_dwTlsIndex);		
		}
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** индекс */
		DWORD _dwTlsIndex = TLS_OUT_OF_INDEXES;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END