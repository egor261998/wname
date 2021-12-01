#include "../stdafx.h"

using CCounterPrefix = wname::misc::CCounter;

//==============================================================================
	void CCounterPrefix::initialize()
	{
		std::lock_guard lock(_mtxCounter);

		if (_isCounterInitialize)
		{
			throw std::logic_error("CCounter already initialize");
		}

		/** создаем событие, если его нет */
		if (!_eventCounterFree.isValid())
			_eventCounterFree.initialize();

		_isCounterInitialize = true;
	}
//==============================================================================
	bool CCounterPrefix::isInitialize()
	{
		std::lock_guard lock(_mtxCounter);

		return _isCounterInitialize;
	}
//==============================================================================
	bool CCounterPrefix::startOperation()
	{
		std::lock_guard lock(_mtxCounter);

		if (_isCounterInitialize)
			_nCounterCount++;

		return _isCounterInitialize;
	}
//==============================================================================
	bool CCounterPrefix::endOperation()
	{
		bool bResultFree = false;
		bool isDelete = false;
		{
			/** отдельная область видимости для синхронизации */
			std::lock_guard lock(_mtxCounter);

			/** проверка на наличие операций */
			assert(_nCounterCount > 0);
			_nCounterCount--;
		
			/** проверка произвольного ожидания */
			if (_counterWait.size() > 0)
			{
				if (const auto it = _counterWait.find(_nCounterCount); it != _counterWait.end())
				{
					for (auto ev : it->second)
					{
						ev.notify();
					}

					_counterWait.erase(it);
				}
			}

			/** завершение последней асинхронной операции */
			bResultFree =
				_nCounterCount == 0 && !_isCounterInitialize;
			isDelete = _isDeleteAfterEndOperation && !checkOperation();
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
	bool CCounterPrefix::checkOperation(
		const uint64_t nCount)
	{
		bool isLast = false;
		{
			std::lock_guard lock(_mtxCounter);
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
			/** отдельная область видимости для синхронизации */
			std::lock_guard lock(_mtxCounter);

			/** ожидание не нужно */
			if (_nCounterCount <= nCount)
				return;

			/** создаем событие */

			ev.initialize();

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

		try
		{
			#pragma warning(disable: 26447)

			/** отдельная область видимости для синхронизации */
			std::lock_guard lock(_mtxCounter);

			_isCounterInitialize = false;
			isNeedWait = _nCounterCount ? true : false;
		}
		catch (const std::exception& ex)
		{
			UNREFERENCED_PARAMETER(ex);

			/** тут смерть */
			assert(false);
		}

		if (isNeedWait)
			_eventCounterFree.wait();
	}
//==============================================================================
	void CCounterPrefix::deleteAfterEndOperation()
	{
		/** отдельная область видимости для синхронизации */
		std::lock_guard lock(_mtxCounter);

		_isDeleteAfterEndOperation = true;
	}
//==============================================================================
	CCounterPrefix::~CCounter()
	{
		release();
	}
//==============================================================================
