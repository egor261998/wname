#pragma once

_WNAME_BEGIN
namespace cs
{
	/** блокировка критической секции пока существует объект */
	class CCriticalSectionScoped final
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		* @param cs - критическая секция.
		*/
		_Acquires_lock_(_cs)
		WNAME CCriticalSectionScoped(
			CCriticalSection& cs) noexcept;
	//==========================================================================
		/**
		* деструктор.
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
		/** используемая критическая секция */
		CCriticalSection& _cs;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END