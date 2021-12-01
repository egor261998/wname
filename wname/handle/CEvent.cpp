#include "../stdafx.h"

using CEventPrefix = wname::handle::CEvent;

//==============================================================================
	CEventPrefix::CEvent(
		const CEvent& handle) noexcept
		:CHandle(handle)
	{
		
	}
//==============================================================================
	CEventPrefix::CEvent(
		CEvent&& handle) noexcept
		:CHandle(std::move(handle))
	{
	
	}
//==============================================================================
	void CEventPrefix::initialize()
	{
		if (isValid())
		{
			throw std::logic_error("CEvent already initialize");
		}

		/** создаем событие */
		const auto hObject = CreateEvent(NULL, FALSE, FALSE, NULL);

		try
		{
			CHandle::operator=(hObject);
		}
		catch (const std::exception&)
		{
			CloseHandle(hObject);
			throw;
		}

		if (!isValid())
		{
			throw std::runtime_error(
				"Create event with error: " + std::to_string(GetLastError()));
		}
	}
//==============================================================================
	std::error_code CEventPrefix::notify() const noexcept
	{
		const auto hObject = getHandle();

		return std::error_code(
			SetEvent(hObject) ? ERROR_SUCCESS : GetLastError(),
			std::system_category());
	}
//==============================================================================
	std::error_code CEventPrefix::wait(
		const DWORD dwMilliseconds) const noexcept
	{
		const auto hObject = getHandle();

		return std::error_code(
			WaitForSingleObject(hObject, dwMilliseconds),
			std::system_category());
	}
//==============================================================================
	std::error_code CEventPrefix::cancel() const noexcept
	{
		const auto hObject = getHandle();

		return std::error_code(
			ResetEvent(hObject) ? ERROR_SUCCESS : GetLastError(),
			std::system_category());
	}
//==============================================================================
	CEventPrefix& CEventPrefix::operator=(
		const CEvent& handle) noexcept
	{
		CHandle::operator=(handle);
		return *this;
	}
//==============================================================================
	CEventPrefix& CEventPrefix::operator=(
		CEvent&& handle) noexcept
	{
		CHandle::operator=(std::move(handle));
		return *this;
	}
//==============================================================================
	CEventPrefix::~CEvent()
	{
		close();
	}
//==============================================================================