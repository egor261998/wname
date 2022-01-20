#pragma once

_WNAME_BEGIN
namespace misc
{
	/** ������� ����������������� ������ */
	class WNAME CInterlockedItem : public SLIST_ENTRY { };

	/** ���������������� ����������� ������ */
	template <class T>
	class WNAME CInterlockedList final
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		CInterlockedList() noexcept
		{
			InitializeSListHead(&_listHead);
		}
	//==========================================================================
		/**
		* �������� ���������� ��������� � ������.
		* ������������ �������� ���������� ����������� ���������.
		* @return - ���������� ���������.
		*/
		size_t size() const noexcept
		{
			return _nCount;
		}
	//==========================================================================
		/**
		* �������� ������.
		*/
		void clear() noexcept
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
		T* const pop() noexcept
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
		std::unique_ptr<T> popEx() noexcept
		{
			return std::unique_ptr<T>(pop());
		}
	//==========================================================================
		/**
		* �������� ������� � ������ ������.
		* @param pInterlockedItem - ����� ������� ������.
		*/
		void push(
			T* const pInterlockedItem) noexcept
		{
			InterlockedPushEntrySList(&_listHead,
				pInterlockedItem);

			_nCount++;
		}
	//==========================================================================
		/**
		* �������� ������� � ������ ������ �� ������ ���������.
		* @param pInterlockedItem - ����� ������� ������ ��� ����� ���������.
		*/
		void pushEx(
			std::unique_ptr<T> pInterlockedItem) noexcept
		{
			push(pInterlockedItem.release());
		}
	//==========================================================================
		/**
		* ����������.
		*/
		~CInterlockedList()
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
		std::atomic_size_t _nCount = 0;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END