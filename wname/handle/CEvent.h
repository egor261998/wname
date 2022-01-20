#pragma once

_WNAME_BEGIN
namespace handle
{
	/** ���������� ������� � ������ */
	class WNAME CEvent final : public CHandle
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		CEvent() noexcept(false);
	//==========================================================================
		/**
		* ����������� �����������.
		* @param handle - ��������� �����������.
		*/
		CEvent(
			const CEvent& handle) noexcept;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param handle - ��������� �����������.
		*/
		CEvent(
			CEvent&& handle) noexcept;
	//==========================================================================
		/**
		* ���� �����������.
		* @return - ��� ������.
		*/
		std::error_code notify() const noexcept;
	//==========================================================================
		/**
		* �������� ����������� ��� ���������� �������.
		* @param bWaitAll - ������� ����.
		* @param dwMilliseconds - ����� �������� �����������.
		* @param args - �������.
		* @return - ��� ������.
		*/
		template<class... Args>
		static std::error_code wait(
			const bool bWaitAll,
			const DWORD dwMilliseconds,
			Args&... args) 
		{
			std::vector<CEvent*> vecEvents({ &args... });
			std::vector<HANDLE> vecHandles;
			const auto numbers = vecEvents.size();
			vecHandles.resize(numbers);
			for (size_t i = 0; i < numbers; i++)
			{
				vecHandles.at(i) = vecEvents.at(i)->getHandle();
			}
			
			return std::error_code(
				WaitForMultipleObjects(
					static_cast<DWORD>(numbers), 
					vecHandles.data(), 
					bWaitAll, 
					dwMilliseconds),
				std::system_category());
		}
	//==========================================================================
		/**
		* �������� �����������.
		* @param dwMilliseconds - ����� �������� �����������.
		* @return - ��� ������.
		*/
		std::error_code wait(
			const DWORD dwMilliseconds = INFINITE) const noexcept;
	//==========================================================================
		/**
		* �������� �����������.
		* @return - ��� ������.
		*/
		std::error_code cancel() const noexcept;
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param handle - ���������� ������.
		* @return - ������� ������.
		*/
		CEvent& operator=(
			const CEvent& handle) noexcept;
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param handle - ������������ ������.
		* @return - ������� ������.
		*/
		CEvent& operator=(
			CEvent&& handle) noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		~CEvent();
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END