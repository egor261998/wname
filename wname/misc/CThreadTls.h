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
		* ����������� �� ���������.
		*/
		CThreadTls() noexcept(false)
		{		
			/** �������� ����� ������ */
			_dwTlsIndex = TlsAlloc();
			if (_dwTlsIndex == TLS_OUT_OF_INDEXES)
			{
				throw std::runtime_error("CThreadTls failed with error: " +
					std::to_string(GetLastError()));
			}
		}
	//==========================================================================
		/**
		* ����������� �����������.
		* @param tls - ������ �����������.
		*/
		CThreadTls(
			const CThreadTls& tls) noexcept
		{
			/** ��������� � ��������� ����������� */
			*this = tls;
		}
	//==========================================================================
		/**
		* ����������� �����������.
		* @param tls - ������ �����������.
		*/
		CThreadTls(
			CThreadTls&& tls) noexcept
		{
			/** ��������� � ��������� ����������� */
			*this = std::move(tls);
		}
	//==========================================================================
		/**
		* ���������� ��������.
		* @param p - ��������.
		* @return - ��������� ���������.
		*/
		bool setValue(T* p) noexcept
		{
			return TlsSetValue(_dwTlsIndex, static_cast<PVOID>(p));
		}
	//==========================================================================
		/**
		* �������� ��������.
		* @return - ��������.
		*/
		T* getValue() const noexcept
		{
			return static_cast<T*>(TlsGetValue(_dwTlsIndex));
		}
	//==========================================================================
		/**
		* �������� ����������� �������.
		* @param tls - ���������� ������.
		* @return - ������� ������.
		*/
		CThreadTls& operator=(
			const CThreadTls& tls) noexcept
		{
			_dwTlsIndex = tls._dwTlsIndex;
			return *this;
		}
	//==========================================================================
		/**
		* �������� ����������� �������.
		* @param tls - ������������ ������.
		* @return - ������� ������.
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
		* ����������.
		*/
		~CThreadTls()
		{
			if (_dwTlsIndex == TLS_OUT_OF_INDEXES)
			{
				/** ������� ���, ������������ ������ */
				return;
			}

			TlsFree(_dwTlsIndex);		
		}
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ������ */
		DWORD _dwTlsIndex = TLS_OUT_OF_INDEXES;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END