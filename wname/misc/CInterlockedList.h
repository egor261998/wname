#pragma once

_WNAME_BEGIN
namespace misc
{
	/** ������� ����������������� ������ */
	class CInterlockedItem : public SLIST_ENTRY {};

	/** ���������������� ����������� ������ */
	template <class T>
	class CInterlockedList
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		WNAME CInterlockedList() noexcept
		{
			InitializeSListHead(&_listHead);
		}
	//==========================================================================
		/**
		* �������� ���������� ��������� � ������.
		* @return - ���������� ���������.
		*
		* ������������ �������� ���������� ����������� ���������.
		*/
		WNAME uint64_t size() const noexcept
		{
			return _nCount;
		}
	//==========================================================================
		/**
		* �������� ������.
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
		* ������� ������ ������� ������.
		* @return - ������� ������.
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
		* ������� ������ ������� ������ ��� ����� ���������.
		* @return - ������� ������ ��� ����� ���������.
		*/
		WNAME std::unique_ptr<T> popEx() noexcept
		{
			return std::unique_ptr<T>(pop());
		}
	//==========================================================================
		/**
		* �������� ������� � ������ ������.
		* @param pInterlockedItem - ����� ������� ������.
		* @return - ����� ������� ������.
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
		* �������� ������� � ������ ������ �� ������ ���������.
		* @param pInterlockedItem - ����� ������� ������ ��� ����� ���������.
		* @return - ����� ������� ������.
		*/
		WNAME T* const pushEx(
			std::unique_ptr<T> pInterlockedItem) noexcept
		{
			return push(pInterlockedItem.release());
		}
	//==========================================================================
		/**
		* ����������.
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
		/** ������ ������ */
		SLIST_HEADER _listHead;

		/** ���������� ��������� � ������ */
		std::atomic_uint64_t _nCount = 0;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END