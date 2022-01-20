#include "../stdafx.h"

using wname::handle::CEvent;

//==============================================================================
CEvent::CEvent() noexcept(false)
{
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
			"Create event failed with error: " + std::to_string(GetLastError()));
	}
}
//==============================================================================
CEvent::CEvent(
	const CEvent& handle) noexcept :
	CHandle(handle)
{
		
}
//==============================================================================
CEvent::CEvent(
	CEvent&& handle) noexcept :
	CHandle(std::move(handle))
{
	
}
//==============================================================================
std::error_code CEvent::notify() const noexcept
{
	const auto hObject = getHandle();

	return std::error_code(
		SetEvent(hObject) ? ERROR_SUCCESS : GetLastError(),
		std::system_category());
}
//==============================================================================
std::error_code CEvent::wait(
	const DWORD dwMilliseconds) const noexcept
{
	const auto hObject = getHandle();

	return std::error_code(
		WaitForSingleObject(hObject, dwMilliseconds),
		std::system_category());
}
//==============================================================================
std::error_code CEvent::cancel() const noexcept
{
	const auto hObject = getHandle();

	return std::error_code(
		ResetEvent(hObject) ? ERROR_SUCCESS : GetLastError(),
		std::system_category());
}
//==============================================================================
CEvent& CEvent::operator=(
	const CEvent& handle) noexcept
{
	CHandle::operator=(handle);
	return *this;
}
//==============================================================================
CEvent& CEvent::operator=(
	CEvent&& handle) noexcept
{
	CHandle::operator=(std::move(handle));
	return *this;
}
//==============================================================================
CEvent::~CEvent()
{
	close();
}
//==============================================================================