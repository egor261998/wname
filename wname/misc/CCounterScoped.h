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
		* @param dwCount - количество повешанных ссылок.
		*/
		WNAME CCounterScoped(
			CCounter& counter,
			const DWORD dwCount = 1) noexcept;
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

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* закрыть все операции.
		* @param bIsEndOpertaion - завершить операции перед закрытием.
		*/
		WNAME void closeOperation(
			const bool bIsEndOpertaion);
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** используемый счетчик */
		CCounter& _counter;

		/** успех старта счетчика */
		std::atomic_uint _dwIsStartOperation = 0;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END