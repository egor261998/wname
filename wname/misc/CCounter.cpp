#include "../stdafx.h"

using CCounterPrefix = wname::misc::CCounter;

//==============================================================================
CCounterPrefix::CCounter() noexcept(false)
{
	
}
//==============================================================================
bool CCounterPrefix::isInitialize() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	return _isCounterInitialize;
}
//==============================================================================
bool CCounterPrefix::startOperation(
	const size_t nCount) noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	if (_isCounterInitialize)
		_nCounterCount+= nCount;

	return _isCounterInitialize;
}
//==============================================================================
bool CCounterPrefix::endOperation(
	const size_t nCount) noexcept
{
	bool bResultFree = false;
	bool isDelete = false;
	{
		/** отдельная область видимости для синхронизации */
		cs::CCriticalSectionScoped lock(_csCounter);

		/** проверка на наличие операций */
		assert(_nCounterCount >= nCount);
		_nCounterCount-= nCount;
		
		/** обработка операции */
		processingOperation(bResultFree, isDelete);
	}

	if (bResultFree)
		_eventCounterFree.notify();

	if (isDelete)
	{
		/** увольняемся */
		delete this;
	}

	return bResultFree;
}
//==============================================================================
void CCounterPrefix::processingOperation(
	bool& bResultFree,
	bool& isDelete) noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);

	/** проверка произвольного ожидания */
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

			/** смерть */
			assert(false);
		}
	}

	/** завершение последней асинхронной операции */
	bResultFree =
		_nCounterCount == 0 && !_isCounterInitialize;
	isDelete = _isDeleteAfterEndOperation && !checkOperation();
}
//==============================================================================
bool CCounterPrefix::checkOperation(
	const size_t nCount) noexcept
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
	const size_t nCount)
{
	handle::CEvent ev;

	{
		/** отдельная область видимости для синхронизации */
		cs::CCriticalSectionScoped lock(_csCounter);

		/** ожидание не нужно */
		if (_nCounterCount <= nCount)
			return;

		std::list<handle::CEvent>* pList = nullptr;

		if (const auto it = _counterWait.find(nCount); it != _counterWait.end())
		{
			pList = &it->second;
		}
		else
		{
			/** элемента нет */
			_counterWait[nCount] = std::list<handle::CEvent>();
			pList = &_counterWait.at(nCount);
		}
		assert(pList != nullptr);

		pList->push_back(ev);
	}

	/** ожидаем */
	ev.wait();
}
//==============================================================================
void CCounterPrefix::release() noexcept
{
	bool isNeedWait = false;

	{
		/** отдельная область видимости для синхронизации */
		cs::CCriticalSectionScoped lock(_csCounter);

		_isCounterInitialize = false;
		isNeedWait = _nCounterCount ? true : false;
	}

	if (isNeedWait)
		_eventCounterFree.wait();
}
//==============================================================================
bool CCounterPrefix::deleteAfterEndOperation() noexcept
{
	bool bResultFree = false;
	bool isDelete = false;
	{
		cs::CCriticalSectionScoped lock(_csCounter);

		_isDeleteAfterEndOperation = true;

		/** обработка операции */
		processingOperation(bResultFree, isDelete);
	}

	if (bResultFree)
		_eventCounterFree.notify();

	if (isDelete)
	{
		/** увольняемся */
		delete this;
	}

	return isDelete;
}
//==============================================================================
CCounterPrefix::~CCounter()
{
	release();
}
//==============================================================================
