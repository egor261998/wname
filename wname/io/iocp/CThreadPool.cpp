#include "../../stdafx.h"

/** объявление через препроцессор, тк класс приватный */
using wname::io::iocp::CIocp;

//==============================================================================
CIocp::CThreadPool::CThreadPool(
	CIocp* const pIocp,
	const DWORD minThreadCount,
	const DWORD maxThreadCount) :
	_pIocp(pIocp)
{
	if (pIocp == nullptr)
	{
		throw std::invalid_argument("pIocp == nullptr");
	}

	try
	{
		if (minThreadCount > maxThreadCount || minThreadCount < 1)
		{
			throw std::invalid_argument("minThreadCount > maxThreadCount || minThreadCount < 1");
		}

		_nMinThreadCount = minThreadCount;
		_nMaxThreadCount = maxThreadCount;

		try
		{
			/** добавляем минимальное количество нитей */
			for (DWORD i = 0; i < minThreadCount; i++)
				canAddWorker(true);
		}
		catch (const std::exception&)
		{
			/** нужно свернуться */
			release(true);

			/** падаем дальше */
			throw;
		}
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(
			logger::EMessageType::critical, ex);

		throw;
	}
}
//==============================================================================
void CIocp::CThreadPool::delWorker(
	const CThreadPoolWorker* const pThreadPoolWorker) noexcept
{
	if (pThreadPoolWorker == nullptr)
		return;

	cs::CCriticalSectionScoped lock(_csCounter);

	if (pThreadPoolWorker->_bIsFree)
		_nFreeThreadCount--;
	_nCurrentThreadCount--;
}
//==============================================================================
bool CIocp::CThreadPool::canFreeWorker(
	CThreadPoolWorker* const pThreadPoolWorker) noexcept
{
	if (pThreadPoolWorker == nullptr)
		return false;

	cs::CCriticalSectionScoped lock(_csCounter);

	/** количество нитей в пуле минимально */
	if (_nCurrentThreadCount <= _nMinThreadCount)
		return false;

	/** остальные нити также высвобождаются, высвободим эту
			и в пуле боле не останется нитей */
	if ((_nCurrentThreadCount - _nFreeThreadCount) <= _nMinThreadCount)
		return false;

	/** освободим эту нить и в пуле будут только занятые нити */
	if ((_nCurrentThreadCount - _nFreeThreadCount - 1) == _nBusyThreadCount)
		return false;

	/** проверка на что на ните не висят операции в обработке */
	if (pThreadPoolWorker->checkOperation())
		return false;

	_nFreeThreadCount++;
	pThreadPoolWorker->_bIsFree = true;

	return true;
}
//==============================================================================
bool CIocp::CThreadPool::canAddWorker(
	bool bAddAnyway)
{
	cs::CCriticalSectionScoped lock(_csCounter);

	if (!bAddAnyway)
		_nBusyThreadCount++;

	/** количество нитей в пуле максимально */
	if (_nCurrentThreadCount >= _nMaxThreadCount && !bAddAnyway)
		return false;

	/** не все нити заняты, новая нить не нужна */
	if ((_nCurrentThreadCount - _nFreeThreadCount) > _nBusyThreadCount &&
		!bAddAnyway)
		return false;

	_nCurrentThreadCount++;

	/** добавляем новую нить */
	try
	{
		if (isInitialize())
			CThreadPoolWorker::start(this);
		else
			return false;
	}
	catch (const std::exception& ex)
	{
		/** если во время создания объекта вызывается исключение
			то необходимо изменить количество нитей обратно */
		_nCurrentThreadCount--;
		_pIocp->log(
			logger::EMessageType::critical, ex);

		throw;
	}

	return true;
}
//==============================================================================
void CIocp::CThreadPool::decrementBusyWorker() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);
	_nBusyThreadCount--;
}
//==============================================================================
void CIocp::CThreadPool::release(
	const bool bIsWait) noexcept
{
	/** завершаем работу */
	__super::release(false);

	{
		/** отдельная область видимости для синхронизации */
		cs::CCriticalSectionScoped lock(_csCounter);

		for (DWORD i = 0; i < _nCurrentThreadCount; i++)
		{
			/** посылаем запрос завершения */
			if (!PostQueuedCompletionStatus(
				_pIocp->_hIocp,
				0,
				(ULONG_PTR)this,
				NULL))
			{
				try
				{
					_pIocp->log(
						logger::EMessageType::warning,
						L"PostQueuedCompletionStatus failed",
						std::error_code(GetLastError(), std::system_category()));
				}
				catch (const std::exception& ex)
				{
					/** отлетели на выделении памяти */
					_pIocp->log(
						logger::EMessageType::warning,
						ex);
				}
			}
		}
	}

	/** ожидать нужно тут */
	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//==============================================================================
CIocp::CThreadPool::~CThreadPool()
{
	release(true);
}
//==============================================================================