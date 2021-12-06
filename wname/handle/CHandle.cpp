#include "../stdafx.h"

using CHandlePrefix = wname::handle::CHandle;

//==============================================================================
CHandlePrefix::CHandle(
	const HANDLE hObject)
{
	/** ��������� ������� ����� ���������� ��������� ���������� */
	*this = hObject;
}
//==============================================================================
CHandlePrefix::CHandle(
	const CHandle& handle) noexcept
{
	/** ��������� ������� ����� ���������� ��������� ����������� */
	*this = handle;
}
//==============================================================================
CHandlePrefix::CHandle(
	CHandle&& handle) noexcept
{
	/** ��������� ������� ����� ���������� ��������� ����������� */
	*this = std::move(handle);
}
//==============================================================================
bool CHandlePrefix::isValid() const noexcept
{
	const auto hObject = getHandle();
	return isValid(hObject);
}
//==============================================================================
bool CHandlePrefix::isValid(
	const HANDLE hObject) noexcept
{
	return (hObject != INVALID_HANDLE_VALUE && hObject != nullptr);
}
//==============================================================================
HANDLE CHandlePrefix::getHandle() const noexcept
{
	return _pHandle.get();
}
//==============================================================================
void CHandlePrefix::close() noexcept
{
	_pHandle.reset();
}
//==============================================================================
CHandlePrefix& CHandlePrefix::operator=(
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
CHandlePrefix& CHandlePrefix::operator=(
	const CHandle& handle) noexcept
{
	/** ��������� ���������� ��������� */
	close();

	_pHandle = handle._pHandle;
	return *this;
}
//==============================================================================
CHandlePrefix& CHandlePrefix::operator=(
	CHandle&& handle) noexcept
{
	/** ��������� ���������� ��������� */
	close();

	_pHandle = std::move(handle._pHandle);
	return *this;
}
//==============================================================================
CHandlePrefix::operator HANDLE() const noexcept
{
	return getHandle();
}
//==============================================================================
CHandlePrefix::~CHandle()
{
	close();
}
//==============================================================================