#pragma once

_WNAME_BEGIN
namespace handle
{
	/** ���������� ������� � ������ */
	class CEvent final : public CHandle
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		WNAME CEvent() noexcept = default;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param handle - ��������� �����������.
		*/
		WNAME CEvent(
			const CEvent& handle) noexcept;
	//==========================================================================
		/**
		* ����������� �����������.
		* @param handle - ��������� �����������.
		*/
		WNAME CEvent(
			CEvent&& handle) noexcept;
	//==========================================================================
		/**
		* ���������������� �������.
		*/
		WNAME void initialize();
	//==========================================================================
		/**
		* ���� �����������.
		* @return - ��� ������.
		*/
		WNAME std::error_code notify() const noexcept;
	//==========================================================================
		template<class... Args>
		WNAME static std::error_code notify(
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
		WNAME std::error_code wait(
			const DWORD dwMilliseconds = INFINITE) const noexcept;
	//==========================================================================
		/**
		* �������� �����������.
		* @return - ��� ������.
		*/
		WNAME std::error_code cancel() const noexcept;
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param handle - ���������� ������.
		* @return - ������� ������.
		*/
		WNAME CEvent& operator=(
			const CEvent& handle) noexcept;
	//==========================================================================
		/**
		* �������� ����������� ���������.
		* @param handle - ������������ ������.
		* @return - ������� ������.
		*/
		WNAME CEvent& operator=(
			CEvent&& handle) noexcept;
	//==========================================================================
		/**
		* ����������.
		*/
		WNAME ~CEvent();
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END