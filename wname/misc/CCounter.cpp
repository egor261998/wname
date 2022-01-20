#include "../stdafx.h"

using wname::misc::CCounter;

//==============================================================================
bool CCounter::isInitialize() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _isCounterInitialize;
}
//==============================================================================
bool CCounter::startOperation(
	const size_t nCount) noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	if (_isCounterInitialize)
		_nCounterCount+= nCount;

	return _isCounterInitialize;
}
//==============================================================================
bool CCounter::endOperation(
	const size_t nCount) noexcept
{
	bool bResultFree = false;
	bool isDelete = false;
	{
		/** ��������� ������� ��������� ��� ������������� */
		cs::CCriticalSectionScoped lock(_csCounter);

		/** �������� �� ������� �������� */
		assert(_nCounterCount >= nCount);
		_nCounterCount-= nCount;
		
		/** ��������� �������� */
		processingOperation(bResultFree, isDelete);
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
void CCounter::processingOperation(
	bool& bResultFree,
	bool& isDelete) noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

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
	bResultFree = _nCounterCount == 0 && !_isCounterInitialize;
	isDelete = _isDeleteAfterEndOperation && !checkOperation();
}
//==============================================================================
bool CCounter::checkOperation(
	const size_t nCount) noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _nCounterCount > nCount;
}
//==============================================================================
void CCounter::waitOperation(
	const size_t nCount)
{
	handle::CEvent ev;

	{
		/** ��������� ������� ��������� ��� ������������� */
		cs::CCriticalSectionScoped lock(_csCounter);

		/** �������� �� ����� */
		if (_nCounterCount <= nCount)
			return;

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
void CCounter::release(
	const bool bIsWait) noexcept
{
	bool isNeedWait = false;

	{
		/** ��������� ������� ��������� ��� ������������� */
		cs::CCriticalSectionScoped lock(_csCounter);

		_isCounterInitialize = false;
		isNeedWait = _nCounterCount ? true : false;
	}

	if (bIsWait && isNeedWait)
		_eventCounterFree.wait();
}
//==============================================================================
bool CCounter::deleteAfterEndOperation() noexcept
{
	bool bResultFree = false;
	bool isDelete = false;
	{
		cs::CCriticalSectionScoped lock(_csCounter);

		_isDeleteAfterEndOperation = true;

		/** ��������� �������� */
		processingOperation(bResultFree, isDelete);
	}

	if (bResultFree)
		_eventCounterFree.notify();

	if (isDelete)
	{
		/** ����������� */
		delete this;
	}

	return isDelete;
}
//==============================================================================
CCounter::~CCounter()
{
	release(true);
}
//==============================================================================
