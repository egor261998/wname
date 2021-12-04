#pragma once

_WNAME_BEGIN
namespace misc
{
	/** ������� �������� */
	class CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* �����������.
		*/
		WNAME CCounter() noexcept { };
	//==========================================================================
		/**
		* ������������� �������� ��������.
		*/
		WNAME virtual void initialize();
	//==========================================================================
		/**
		* ��������������� �� ������� ��������.
		*/
		WNAME bool isInitialize() noexcept;
	//==========================================================================
		/**
		* ����� ����� ��������.
		* @return - ����� ������ ��������.
		*/
		WNAME bool startOperation() noexcept;
	//==========================================================================
		/**
		* ��������� ��������.
		* @return - ����������� ����������.
		*/
		WNAME bool endOperation() noexcept;
	//==========================================================================
		/**
		* �������� ���������� ��������.
		* @param nCount - �������� ������.
		* @return - TRUE ���� �������� � ��������� ������ ��� ������� � ���������.
		*/
		WNAME bool checkOperation(
			const uint64_t nCount = 0) noexcept;
	//==========================================================================
		/**
		* �������� ��������.
		* @param nCount - �������� ������.
		*/
		WNAME void waitOperation(
			const uint64_t nCount = 0);
	//==========================================================================
		/**
		* ��������� ������ � ��������� ���� ��������.
		*/
		WNAME virtual void release() noexcept;
	//==========================================================================
		/**
		* �������� ������� ����� ���� ��� ���������� ��������� ��������.
		*/
		WNAME void deleteAfterEndOperation() noexcept;
	//==========================================================================
		/**
		* ����������.
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
		/** ��������� */
		bool _isCounterInitialize = false;
		bool _isDeleteAfterEndOperation = false;

		/** ������������ �������� �������� */
		std::unordered_map<uint64_t, std::list<handle::CEvent>> _counterWait;

		/** ���������� ����������� ����������� �������� */
		uint64_t _nCounterCount = 0;

		/** ������� ���������� ��������� �������� */
		handle::CEvent _eventCounterFree;

		/** ������������� �������� */
		cs::CCriticalSection _csCounter;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END