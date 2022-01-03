#pragma once

_WNAME_BEGIN
namespace cs
{
	/** ���������� ����������� ������ ���� ���������� ������ */
	class CCriticalSectionScoped final
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		* @param cs - ����������� ������.
		*/
		_Acquires_lock_(_cs)
		WNAME CCriticalSectionScoped(
			CCriticalSection& cs) noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		_Releases_lock_(_cs)
		WNAME ~CCriticalSectionScoped();
	//==========================================================================
		CCriticalSectionScoped(const CCriticalSectionScoped&) = delete;
		CCriticalSectionScoped(CCriticalSectionScoped&&) = delete;
		CCriticalSectionScoped& operator=(const CCriticalSectionScoped&) = delete;
		CCriticalSectionScoped& operator=(CCriticalSectionScoped&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ������������ ����������� ������ */
		CCriticalSection& _cs;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END