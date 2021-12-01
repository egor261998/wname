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
		WNAME CCounter() noexcept = default;
	//==========================================================================
		/**
		* ������������� �������� ��������.
		*/
		WNAME virtual void initialize();
	//==========================================================================
		/**
		* ��������������� �� ������� ��������.
		*/
		WNAME bool isInitialize();
	//==========================================================================
		/**
		* ����� ����� ��������.
		* @return - ����� ������ ��������.
		*/
		WNAME bool startOperation();
	//==========================================================================
		/**
		* ��������� ��������.
		* @return - ����������� ����������.
		*/
		WNAME bool endOperation();
	//==========================================================================
		/**
		* �������� ���������� ��������.
		* @param nCount - �������� ������.
		* @return - TRUE ���� �������� � ��������� ������ ��� ������� � ���������.
		*/
		WNAME bool checkOperation(
			const uint64_t nCount = 0);
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
		WNAME void deleteAfterEndOperation();
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

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ��������� */
		bool _isCounterInitialize = false;
		bool _isDeleteAfterEndOperation = false;

		/** ������������ �������� �������� */
		std::map<uint64_t, std::list<handle::CEvent>> _counterWait;

		/** ���������� ����������� ����������� �������� */
		uint64_t _nCounterCount = 0;

		/** ������� ���������� ��������� �������� */
		handle::CEvent _eventCounterFree;

		/** ������������� �������� */
		std::recursive_mutex _mtxCounter;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END