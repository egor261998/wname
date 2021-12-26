#pragma once

_WNAME_BEGIN
namespace misc
{
	/** ������� �������� RAII */
	class CCounterScoped
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		* @param counter - ������� ��������.
		* @param dwCount - ���������� ���������� ������.
		*/
		WNAME CCounterScoped(
			CCounter& counter,
			const DWORD dwCount = 1) noexcept;
	//==========================================================================
		/**
		* ����� ������ ��������.
		* @return - ����� ������ ��������.
		*/
		WNAME bool isStartOperation() const noexcept;
	//==========================================================================
		/**
		* ���������� ��� ���������� ��������.
		*/
		WNAME void release() noexcept;
	//==========================================================================
		/**
		* ����������.
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
		* ������� ��� ��������.
		* @param bIsEndOpertaion - ��������� �������� ����� ���������.
		*/
		WNAME void closeOperation(
			const bool bIsEndOpertaion) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ������������ ������� */
		CCounter& _counter;

		/** ����� ������ �������� */
		std::atomic_uint _dwIsStartOperation = 0;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END