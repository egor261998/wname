#pragma once

_WNAME_BEGIN
namespace misc
{
	/** ������� �������� */
	class WNAME CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* �����������.
		*/
		CCounter() noexcept(false) = default;
	//==========================================================================
		/**
		* ��������������� �� ������� ��������.
		*/
		bool isInitialize() noexcept;
	//==========================================================================
		/**
		* ����� ����� ��������.
		* @param nCount - ���������� ��������.
		* @return - ����� ������ ��������.
		*/
		bool startOperation(
			const size_t nCount = 1) noexcept;
	//==========================================================================
		/**
		* ��������� ��������.
		* @param nCount - ���������� ��������.
		* @return - ����������� ����������.
		*/
		bool endOperation(
			const size_t nCount = 1) noexcept;
	//==========================================================================
		/**
		* �������� ���������� ��������.
		* @param nCount - �������� ������.
		* @return - TRUE ���� �������� � ��������� ������ ��� ������� � ���������.
		*/
		bool checkOperation(
			const size_t nCount = 0) noexcept;
	//==========================================================================
		/**
		* �������� ��������.
		* @param nCount - �������� ������.
		*/
		void waitOperation(
			const size_t nCount = 0);
	//==========================================================================
		/**
		* ��������� ������ � ��������� ��������.
		* @param bIsWait - ������� ��������.
		*/
		virtual void release(
			const bool bIsWait) noexcept;
	//==========================================================================
		/**
		* �������� ������� ����� ���� ��� ���������� ��������� ��������.
		* @return - ������� ���� ��� ������ ��������.
		*/
		bool deleteAfterEndOperation() noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		virtual ~CCounter();
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
		void processingOperation(
			bool& bResultFree,
			bool& isDelete) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================	
		/** ������������ �������� �������� */
		std::map<size_t, std::list<handle::CEvent>> _counterWait;

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