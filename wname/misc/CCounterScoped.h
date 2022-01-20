#pragma once

_WNAME_BEGIN
namespace misc
{
	/** ������� �������� RAII */
	class WNAME CCounterScoped final
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		* @param counter - ������� ��������.
		* @param nCount - ���������� ��������.
		*/
		CCounterScoped(
			CCounter& counter,
			const size_t nCount = 1) noexcept;
	//==========================================================================
		/**
		* ����� ������ ��������.
		* @return - ����� ������ ��������.
		*/
		bool isStartOperation() const noexcept;
	//==========================================================================
		/**
		* ���������� ��� ������ ������.
		*/
		void release() noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		~CCounterScoped();
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
		size_t _nCountStart = 0;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END