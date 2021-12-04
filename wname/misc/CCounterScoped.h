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
		*/
		WNAME CCounterScoped(
			CCounter& counter) noexcept;
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

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ������������ ������� */
		CCounter& _counter;

		/** ����� ������ �������� */
		std::atomic_bool _bIsStartOperation = false;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END