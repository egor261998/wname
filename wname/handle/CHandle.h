#pragma once

_WNAME_BEGIN
namespace handle
{
	/** ���������� ��������� � ������ */
	class WNAME CHandle
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		CHandle() noexcept = default;
	//==========================================================================
		/**
		* ����������� �� ���������.
		* @param hObject - ����� ���������.
		*/
		CHandle(
			const HANDLE hObject);
	//==========================================================================
		/**
		* ����������� �����������.
		* @param handle - ��������� �����������.
		*/
		CHandle(
			const CHandle& handle) noexcept;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param handle - ��������� �����������.
		*/
		CHandle(
			CHandle&& handle) noexcept;
	//==========================================================================
		/**
		* ��������� ����������.
		* @return - ��������� ��������.
		*/
		bool isValid() const noexcept;
	//==========================================================================
		/**
		* ��������� ����������.
		* @param hObject - ���������.
		* @return - ��������� ��������.
		*/
		static bool isValid(
			const HANDLE hObject) noexcept;
	//==========================================================================
		/**
		* �������� ���������.
		* @return - ���������.
		*/
		HANDLE getHandle() const noexcept;
	//==========================================================================
		/**
		* ������� ���������.
		*/
		void close() noexcept;
	//==========================================================================
		/**
		* �������� ���������� ���������.
		* @param hObject - ����� ���������.
		* @return - ������� ������.
		*/
		virtual CHandle& operator=(
			const HANDLE hObject);
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param handle - ���������� ������.
		* @return - ������� ������.
		*/
		virtual CHandle& operator=(
			const CHandle& handle) noexcept;
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param handle - ������������ ������.
		* @return - ������� ������.
		*/
		virtual CHandle& operator=(
			CHandle&& handle) noexcept;
	//==========================================================================
		/**
		* �������� ��������� ���������.
		* @return - ���������.
		*/
		operator HANDLE() const noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		virtual ~CHandle();
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ��������� */
		std::shared_ptr<void> _pHandle;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END