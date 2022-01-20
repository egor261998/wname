#include "../stdafx.h"

using wname::handle::CHandle;

//==============================================================================
CHandle::CHandle(
	const HANDLE hObject)
{
	/** выполняем команду через перегрузку оператора присвоения */
	*this = hObject;
}
//==============================================================================
CHandle::CHandle(
	const CHandle& handle) noexcept
{
	/** выполняем команду через перегрузку оператора копирования */
	*this = handle;
}
//==============================================================================
CHandle::CHandle(
	CHandle&& handle) noexcept
{
	/** выполняем команду через перегрузку оператора перемещения */
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
	/** закрываем предыдущий описатель */
	close();

	if (isValid(hObject))
	{
		/** описатель валидный */
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