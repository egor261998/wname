#pragma once

_WNAME_BEGIN
namespace cs
{
	/** реализация критических секций в классе */
	class WNAME CCriticalSection final : 
		private CRITICAL_SECTION
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		*/
		CCriticalSection() noexcept;
	//==========================================================================
		/**
		* блокировка.
		*/
		_Acquires_lock_(*this)
		 void lock() noexcept;
	//==========================================================================
		/**
		* попытка блокировки.
		* @return - успех блокировки.
		*/
		_Acquires_lock_(*this)
		_Success_(return != false)
		bool tryLock() noexcept;
	//==========================================================================
		/**
		* разблокировка.
		*/
		_Releases_lock_(*this)
		void unLock() noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		~CCriticalSection();
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