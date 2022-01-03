#pragma once

_WNAME_BEGIN
namespace cs
{
	/** реализация критических секций в классе */
	class CCriticalSection final : private CRITICAL_SECTION
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		*/
		WNAME CCriticalSection() noexcept;
	//==========================================================================
		/**
		* блокировка.
		*/
		_Acquires_lock_(*this)
		WNAME void lock() noexcept;
	//==========================================================================
		/**
		* попытка блокировки.
		* @return - успех блокировки.
		*/
		_Acquires_lock_(*this)
		_Success_(return != false)
		WNAME bool tryLock() noexcept;
	//==========================================================================
		/**
		* разблокировка.
		*/
		_Releases_lock_(*this)
		WNAME void unlock() noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CCriticalSection();
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