#pragma once

_WNAME_BEGIN
namespace cs
{
	/** ���������� ����������� ������ � ������ */
	class CCriticalSection : private CRITICAL_SECTION
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		WNAME CCriticalSection() noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		_Acquires_lock_(*this)
		WNAME virtual void lock() noexcept;
	//==========================================================================
		/**
		* ������� ����������.
		* @return - ����� ����������.
		*/
		_Acquires_lock_(*this)
		_Success_(return != false)
		WNAME virtual bool tryLock() noexcept;
	//==========================================================================
		/**
		* �������������.
		*/
		_Releases_lock_(*this)
		WNAME virtual void unlock() noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		WNAME virtual ~CCriticalSection();
	//==========================================================================
		CCriticalSection(const CCriticalSection&) = delete;
		CCriticalSection(CCriticalSection&&) = delete;
		CCriticalSection& operator=(const CCriticalSection&) = delete;
		CCriticalSection& operator=(CCriticalSection&&) = delete;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END