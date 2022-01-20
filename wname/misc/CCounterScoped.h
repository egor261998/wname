#pragma once

_WNAME_BEGIN
namespace misc
{
	/** счетчик операций RAII */
	class WNAME CCounterScoped final
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		* @param counter - счетчик операций.
		* @param nCount - количество операций.
		*/
		CCounterScoped(
			CCounter& counter,
			const size_t nCount = 1) noexcept;
	//==========================================================================
		/**
		* успех старта счетчика.
		* @return - успех старта операции.
		*/
		bool isStartOperation() const noexcept;
	//==========================================================================
		/**
		* освободить без снятия ссылок.
		*/
		void release() noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		~CCounterScoped();
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
		size_t _nCountStart = 0;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END