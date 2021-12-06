#include "../stdafx.h"

using CCounterPrefix = wname::misc::CCounter;

//==============================================================================
void CCounterPrefix::initialize()
{
	cs::CCriticalSectionScoped lock(_csCounter);

	if (_isCounterInitialize)
	{
		throw std::logic_error("CCounter already initialize");
	}

	/** ������� �������, ���� ��� ��� */
	if (!_eventCounterFree.isValid())
		_eventCounterFree.initialize();

	_isCounterInitialize = true;
}
//==============================================================================
bool CCounterPrefix::isInitialize() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _isCounterInitialize;
}
//==============================================================================
bool CCounterPrefix::startOperation() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	if (_isCounterInitialize)
		_nCounterCount++;

	return _isCounterInitialize;
}
//==============================================================================
bool CCounterPrefix::endOperation() noexcept
{
	bool bResultFree = false;
	bool isDelete = false;
	{
		/** ��������� ������� ��������� ��� ������������� */
		cs::CCriticalSectionScoped lock(_csCounter);

		/** �������� �� ������� �������� */
		assert(_nCounterCount > 0);
		_nCounterCount--;
		
		/** �������� ������������� �������� */		
		if (!_counterWait.empty())
		{
			try
			{
				if (const auto& it = _counterWait.find(_nCounterCount);
					it != _counterWait.end())
				{
					for (const auto& ev : it->second)
					{
						ev.notify();
					}

					_counterWait.erase(it);
				}
			}
			catch (const std::exception& ex)
			{
				UNREFERENCED_PARAMETER(ex);

				/** ������ */
				assert(false);
			}
		}	

		/** ���������� ��������� ����������� �������� */
		bResultFree =
			_nCounterCount == 0 && !_isCounterInitialize;
		isDelete = _isDeleteAfterEndOperation && !checkOperation();
	}

	if (bResultFree)
		_eventCounterFree.notify();

	if (isDelete)
	{
		/** ����������� */
		delete this;
	}

	return bResultFree;
}
//==============================================================================
bool CCounterPrefix::checkOperation(
	const uint64_t nCount) noexcept
{
	bool isLast = false;
	{
		cs::CCriticalSectionScoped lock(_csCounter);
		isLast = _nCounterCount > nCount;
	}

	return isLast;
}
//==============================================================================
void CCounterPrefix::waitOperation(
	const uint64_t nCount)
{
	handle::CEvent ev;

	{
		/** ��������� ������� ��������� ��� ������������� */
		cs::CCriticalSectionScoped lock(_csCounter);

		/** �������� �� ����� */
		if (_nCounterCount <= nCount)
			return;

		/** ������� ������� */
		ev.initialize();

		std::list<handle::CEvent>* pList = nullptr;

		if (const auto it = _counterWait.find(nCount); it != _counterWait.end())
		{
			pList = &it->second;
		}
		else
		{
			/** �������� ��� */
			_counterWait[nCount] = std::list<handle::CEvent>();
			pList = &_counterWait.at(nCount);
		}
		assert(pList != nullptr);

		pList->push_back(ev);
	}

	/** ������� */
	ev.wait();
}
//==============================================================================
void CCounterPrefix::release() noexcept
{
	bool isNeedWait = false;

	/** ��������� ������� ��������� ��� ������������� */
	cs::CCriticalSectionScoped lock(_csCounter);

	_isCounterInitialize = false;
	isNeedWait = _nCounterCount ? true : false;

	if (isNeedWait)
		_eventCounterFree.wait();
}
//==============================================================================
void CCounterPrefix::deleteAfterEndOperation() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	_isDeleteAfterEndOperation = true;
}
//==============================================================================
CCounterPrefix::~CCounter()
{
	release();
}
//==============================================================================
