#pragma once

_WNAME_BEGIN
namespace cs
{
	/** ���������� ����������� ������ � ������ */
	class WNAME CCriticalSection final : 
		private CRITICAL_SECTION
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		CCriticalSection() noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		_Acquires_lock_(*this)
		 void lock() noexcept;
	//==========================================================================
		/**
		* ������� ����������.
		* @return - ����� ����������.
		*/
		_Acquires_lock_(*this)
		_Success_(return != false)
		bool tryLock() noexcept;
	//==========================================================================
		/**
		* �������������.
		*/
		_Releases_lock_(*this)
		void unLock() noexcept;
	//==========================================================================
		/**
		* ����������.
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