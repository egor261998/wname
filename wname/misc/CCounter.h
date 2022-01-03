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
		WNAME CCounter() noexcept(false);
	//==========================================================================
		/**
		* ��������������� �� ������� ��������.
		*/
		WNAME bool isInitialize() noexcept;
	//==========================================================================
		/**
		* ����� ����� ��������.
		* @param nCount - ���������� ��������.
		* @return - ����� ������ ��������.
		*/
		WNAME bool startOperation(
			const size_t nCount = 1) noexcept;
	//==========================================================================
		/**
		* ��������� ��������.
		* @param nCount - ���������� ��������.
		* @return - ����������� ����������.
		*/
		WNAME bool endOperation(
			const size_t nCount = 1) noexcept;
	//==========================================================================
		/**
		* �������� ���������� ��������.
		* @param nCount - �������� ������.
		* @return - TRUE ���� �������� � ��������� ������ ��� ������� � ���������.
		*/
		WNAME bool checkOperation(
			const size_t nCount = 0) noexcept;
	//==========================================================================
		/**
		* �������� ��������.
		* @param nCount - �������� ������.
		*/
		WNAME void waitOperation(
			const size_t nCount = 0);
	//==========================================================================
		/**
		* ��������� ������ � ��������� ���� ��������.
		*/
		WNAME virtual void release() noexcept;
	//==========================================================================
		/**
		* �������� ������� ����� ���� ��� ���������� ��������� ��������.
		* @return - ������ ��������.
		*/
		WNAME bool deleteAfterEndOperation() noexcept;
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

	#pragma region Private_Method
	private:
	//==========================================================================		
		/**
		* ��������� ��������.
		* @param bResultFree - ������� ����������.
		* @param isDelete - ����� �� ������� ������.
		*/
		WNAME void processingOperation(
			bool& bResultFree,
			bool& isDelete) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================	
		/** ������������ �������� �������� */
		std::unordered_map<uint64_t, std::list<handle::CEvent>> _counterWait;

		/** ���������� ����������� ����������� �������� */
		size_t _nCounterCount = 0;

		/** ������� ���������� ��������� �������� */
		handle::CEvent _eventCounterFree;

		/** ��������� */
		bool _isCounterInitialize = true;
		bool _isDeleteAfterEndOperation = false;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================	
		/** ������������� �������� */
		cs::CCriticalSection _csCounter;	
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END