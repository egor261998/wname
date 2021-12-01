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
		* ����������� �� ���������.
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
		* ����������� �����������.
		* @param tls - ������ �����������.
		*/
		WNAME CThreadTls(
			const CThreadTls& tls) noexcept
		{
			*this = tls;
		}
	//==========================================================================
		/**
		* ����������� �����������.
		* @param tls - ������ �����������.
		*/
		WNAME CThreadTls(
			CThreadTls&& tls) noexcept
		{
			*this = std::move(tls);
		}
	//==========================================================================
		/**
		* ���������� ��������.
		* @param p - ��������.
		* @return - ��������� ���������.
		*/
		WNAME bool setValue(T* p) noexcept
		{
			return TlsSetValue(_dwTlsIndex, static_cast<PVOID>(p));
		}
	//==========================================================================
		/**
		* �������� ��������.
		* @return - ��������.
		*/
		WNAME T* getValue() noexcept
		{
			return static_cast<T*>(TlsGetValue(_dwTlsIndex));
		}
	//==========================================================================
		/**
		* �������� ����������� �������.
		* @param tls - ���������� ������.
		* @return - ������� ������.
		*/
		WNAME CThreadTls& operator=(
			const CThreadTls& tls) noexcept
		{
			_dwTlsIndex = tls._dwTlsIndex;
		}
	//==========================================================================
		/**
		* �������� ����������� �������.
		* @param tls - ������������ ������.
		* @return - ������� ������.
		*/
		WNAME CThreadTls& operator=(
			CThreadTls&& tls) noexcept
		{
			_dwTlsIndex = tls._dwTlsIndex;
			tls._dwTlsIndex = TLS_OUT_OF_INDEXES;
		}
	//==========================================================================
		/**
		* ����������.
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
		/** ����������� ������ */
		DWORD _dwTlsIndex = TLS_OUT_OF_INDEXES;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END