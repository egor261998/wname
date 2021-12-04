#pragma once

_WNAME_BEGIN
namespace misc
{
	/** элемент потокобезопасного списка */
	class CInterlockedItem : public SLIST_ENTRY {};

	/** потокобезопасный односвязный список */
	template <class T>
	class CInterlockedList
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		*/
		WNAME CInterlockedList() noexcept
		{
			InitializeSListHead(&_listHead);
		}
	//==========================================================================
		/**
		* получить количество элементов в списке.
		* @return - количество элементов.
		*
		* возвращаемое значение подвержено мгновенному изменению.
		*/
		WNAME uint64_t size() const noexcept
		{
			return _nCount;
		}
	//==========================================================================
		/**
		* очистить список.
		*/
		WNAME void clear() noexcept
		{
			while (true)
			{
				auto pInterlockedItem = popEx();

				if (pInterlockedItem == nullptr)
					return;
			}
		}
	//==========================================================================
		/**
		* извлечь первый элемент списка.
		* @return - элемент списка.
		*/
		WNAME T* const pop() noexcept
		{
			#pragma warning (disable: 26481)
			auto pItemEntry = InterlockedPopEntrySList(&_listHead);

			if (pItemEntry == nullptr)
				return nullptr;

			_nCount--;

			return CONTAINING_RECORD(pItemEntry, T, Next);
		}
	//==========================================================================
		/**
		* извлечь первый элемент списка как умный указатель.
		* @return - элемент списка как умный указатель.
		*/
		WNAME std::unique_ptr<T> popEx() noexcept
		{
			return std::unique_ptr<T>(pop());
		}
	//==========================================================================
		/**
		* вставить элемент в начало списка.
		* @param pInterlockedItem - новый элемент списка.
		* @return - новый элемент списка.
		*/
		WNAME T* const push(
			T* const pInterlockedItem) noexcept
		{
			PSLIST_ENTRY pItem = pInterlockedItem;

			InterlockedPushEntrySList(&_listHead,
				pItem);

			_nCount++;

			return pInterlockedItem;
		}
	//==========================================================================
		/**
		* вставить элемент в начало списка из умного указателя.
		* @param pInterlockedItem - новый элемент списка как умный указатель.
		* @return - новый элемент списка.
		*/
		WNAME T* const pushEx(
			std::unique_ptr<T> pInterlockedItem) noexcept
		{
			return push(pInterlockedItem.release());
		}
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CInterlockedList()
		{
			clear();
		}
	//==========================================================================
		CInterlockedList(const CInterlockedList&) = delete;
		CInterlockedList(CInterlockedList&&) = delete;
		CInterlockedList& operator=(const CInterlockedList&) = delete;
		CInterlockedList& operator=(CInterlockedList&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** голова списка */
		SLIST_HEADER _listHead;

		/** количество элементов в списке */
		std::atomic_uint64_t _nCount = 0;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END