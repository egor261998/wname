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
		WNAME CCounter() noexcept(false);
	//==========================================================================
		/**
		* инициализирован ли счетчик операций.
		*/
		WNAME bool isInitialize() noexcept;
	//==========================================================================
		/**
		* старт новой операции.
		* @param nCount - количество операций.
		* @return - успех старта операции.
		*/
		WNAME bool startOperation(
			const size_t nCount = 1) noexcept;
	//==========================================================================
		/**
		* окончание операции.
		* @param nCount - количество операций.
		* @return - возможность завершения.
		*/
		WNAME bool endOperation(
			const size_t nCount = 1) noexcept;
	//==========================================================================
		/**
		* проверка количества операций.
		* @param nCount - смещение ссылок.
		* @return - TRUE если операций в обработке больше чем указано в параметре.
		*/
		WNAME bool checkOperation(
			const size_t nCount = 0) noexcept;
	//==========================================================================
		/**
		* ожидание операций.
		* @param nCount - смещение ссылок.
		*/
		WNAME void waitOperation(
			const size_t nCount = 0);
	//==========================================================================
		/**
		* закончить работу и дождаться всех операций.
		*/
		WNAME virtual void release() noexcept;
	//==========================================================================
		/**
		* удаление объекта после того как отработает последняя операция.
		* @return - объект удалился.
		*/
		WNAME bool deleteAfterEndOperation() noexcept;
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

	#pragma region Private_Method
	private:
	//==========================================================================		
		/**
		* обработка операции.
		* @param bResultFree - признак завершения.
		* @param isDelete - нужно ли удалять объект.
		*/
		WNAME void processingOperation(
			bool& bResultFree,
			bool& isDelete) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================	
		/** произвольное ожидание операции */
		std::unordered_map<uint64_t, std::list<handle::CEvent>> _counterWait;

		/** количество выполняемых асинхронных операций */
		size_t _nCounterCount = 0;

		/** событие завершения последней операции */
		handle::CEvent _eventCounterFree;

		/** состояние */
		bool _isCounterInitialize = true;
		bool _isDeleteAfterEndOperation = false;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================	
		/** синхронизация счетчика */
		cs::CCriticalSection _csCounter;	
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END