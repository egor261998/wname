#include "../stdafx.h"

using wname::handle::CHandle;

//==============================================================================
CHandle::CHandle(
	const HANDLE hObject)
{
	/** ��������� ������� ����� ���������� ��������� ���������� */
	*this = hObject;
}
//==============================================================================
CHandle::CHandle(
	const CHandle& handle) noexcept
{
	/** ��������� ������� ����� ���������� ��������� ����������� */
	*this = handle;
}
//==============================================================================
CHandle::CHandle(
	CHandle&& handle) noexcept
{
	/** ��������� ������� ����� ���������� ��������� ����������� */
	*this = std::move(handle);
}
//==============================================================================
bool CHandle::isValid() const noexcept
{
	const auto hObject = getHandle();
	return isValid(hObject);
}
//==============================================================================
bool CHandle::isValid(
	const HANDLE hObject) noexcept
{
	return (hObject != INVALID_HANDLE_VALUE && hObject != nullptr);
}
//==============================================================================
HANDLE CHandle::getHandle() const noexcept
{
	return _pHandle.get();
}
//==============================================================================
void CHandle::close() noexcept
{
	_pHandle.reset();
}
//==============================================================================
CHandle& CHandle::operator=(
	const HANDLE hObject)
{
	/** ��������� ���������� ��������� */
	close();

	if (isValid(hObject))
	{
		/** ��������� �������� */
		_pHandle = std::shared_ptr<void>(hObject, &CloseHandle);
		return *this;
	}

	return *this;
}
//==============================================================================
CHandle& CHandle::operator=(
	const CHandle& handle) noexcept
{
	_pHandle = handle._pHandle;
	return *this;
}
//==============================================================================
CHandle& CHandle::operator=(
	CHandle&& handle) noexcept
{
	_pHandle = std::move(handle._pHandle);
	return *this;
}
//==============================================================================
CHandle::operator HANDLE() const noexcept
{
	return getHandle();
}
//==============================================================================
CHandle::~CHandle()
{
	close();
}
//==============================================================================