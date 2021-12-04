#pragma once

_WNAME_BEGIN
namespace misc
{
	/** счетчик операций RAII */
	class CCounterScoped
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		* @param counter - счетчик операций.
		*/
		WNAME CCounterScoped(
			CCounter& counter) noexcept;
	//==========================================================================
		/**
		* успех старта счетчика.
		* @return - успех старта операции.
		*/
		WNAME bool isStartOperation() const noexcept;
	//==========================================================================
		/**
		* освободить без уменьшение счетчика.
		*/
		WNAME void release() noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME virtual ~CCounterScoped();
	//==========================================================================
		CCounterScoped(const CCounterScoped&) = delete;
		CCounterScoped(CCounterScoped&&) = delete;
		CCounterScoped& operator=(const CCounterScoped&) = delete;
		CCounterScoped& operator=(CCounterScoped&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** используемый счетчик */
		CCounter& _counter;

		/** успех старта счетчика */
		std::atomic_bool _bIsStartOperation = false;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END