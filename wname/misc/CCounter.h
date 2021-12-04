#pragma once

_WNAME_BEGIN
namespace misc
{
	/** счетчик операций */
	class CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор.
		*/
		WNAME CCounter() noexcept { };
	//==========================================================================
		/**
		* инициализация счетчика операций.
		*/
		WNAME virtual void initialize();
	//==========================================================================
		/**
		* инициализирован ли счетчик операций.
		*/
		WNAME bool isInitialize() noexcept;
	//==========================================================================
		/**
		* старт новой операции.
		* @return - успех старта операции.
		*/
		WNAME bool startOperation() noexcept;
	//==========================================================================
		/**
		* окончание операции.
		* @return - возможность завершения.
		*/
		WNAME bool endOperation() noexcept;
	//==========================================================================
		/**
		* проверка количества операций.
		* @param nCount - смещение ссылок.
		* @return - TRUE если операций в обработке больше чем указано в параметре.
		*/
		WNAME bool checkOperation(
			const uint64_t nCount = 0) noexcept;
	//==========================================================================
		/**
		* ожидание операций.
		* @param nCount - смещение ссылок.
		*/
		WNAME void waitOperation(
			const uint64_t nCount = 0);
	//==========================================================================
		/**
		* закончить работу и дождаться всех операций.
		*/
		WNAME virtual void release() noexcept;
	//==========================================================================
		/**
		* удаление объекта после того как отработает последняя операция.
		*/
		WNAME void deleteAfterEndOperation() noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME virtual ~CCounter();
	//==========================================================================
		CCounter(const CCounter&) = delete;
		CCounter(CCounter&&) = delete;
		CCounter& operator=(const CCounter&) = delete;
		CCounter& operator=(CCounter&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** состояние */
		bool _isCounterInitialize = false;
		bool _isDeleteAfterEndOperation = false;

		/** произвольное ожидание операции */
		std::unordered_map<uint64_t, std::list<handle::CEvent>> _counterWait;

		/** количество выполняемых асинхронных операций */
		uint64_t _nCounterCount = 0;

		/** событие завершения последней операции */
		handle::CEvent _eventCounterFree;

		/** синхронизация счетчика */
		cs::CCriticalSection _csCounter;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END