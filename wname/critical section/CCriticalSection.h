#pragma once

_WNAME_BEGIN
namespace cs
{
	/** реализация критических секций в классе */
	class CCriticalSection : private CRITICAL_SECTION
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
		WNAME virtual void lock() noexcept;
	//==========================================================================
		/**
		* попытка блокировки.
		* @return - успех блокировки.
		*/
		_Acquires_lock_(*this)
		_Success_(return != false)
		WNAME virtual bool tryLock() noexcept;
	//==========================================================================
		/**
		* разблокировка.
		*/
		_Releases_lock_(*this)
		WNAME virtual void unlock() noexcept;
	//==========================================================================
		/**
		* деструктор.
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