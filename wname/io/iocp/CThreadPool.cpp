#include "../../stdafx.h"

/** ���������� ����� ������������, �� ����� ��������� */
#define CThreadPoolPrefix wname::io::iocp::CIocp::CThreadPool

//==============================================================================
CThreadPoolPrefix::CThreadPool(
	CIocp* const pIocp,
	const DWORD minThreadCount,
	const DWORD maxThreadCount)
:_pIocp(pIocp)
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

		/** �������������� ������� */
		initialize();

		try
		{
			/** ��������� ����������� ���������� ����� */
			for (DWORD i = 0; i < minThreadCount; i++)
				canAddWorker(true);
		}
		catch (const std::exception&)
		{
			/** ����� ���������� */
			release();

			/** ������ ������ */
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
void CThreadPoolPrefix::delWorker(
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
bool CThreadPoolPrefix::canFreeWorker(
	CThreadPoolWorker* const pThreadPoolWorker) noexcept
{
	if (pThreadPoolWorker == nullptr)
		return false;

	cs::CCriticalSectionScoped lock(_csCounter);

	/** ���������� ����� � ���� ���������� */
	if (_nCurrentThreadCount <= _nMinThreadCount)
		return false;

	/** ��������� ���� ����� ��������������, ���������� ���
			� � ���� ���� �� ��������� ����� */
	if ((_nCurrentThreadCount - _nFreeThreadCount) <= _nMinThreadCount)
		return false;

	/** ��������� ��� ���� � � ���� ����� ������ ������� ���� */
	if ((_nCurrentThreadCount - _nFreeThreadCount - 1) == _nBusyThreadCount)
		return false;

	/** �������� �� ��� �� ���� �� ����� �������� � ��������� */
	if (pThreadPoolWorker->checkOperation())
		return false;

	_nFreeThreadCount++;
	pThreadPoolWorker->_bIsFree = true;

	return true;
}
//==============================================================================
bool CThreadPoolPrefix::canAddWorker(
	bool bAddAnyway)
{
	cs::CCriticalSectionScoped lock(_csCounter);

	if (!bAddAnyway)
		_nBusyThreadCount++;

	/** ���������� ����� � ���� ����������� */
	if (_nCurrentThreadCount >= _nMaxThreadCount && !bAddAnyway)
		return false;

	/** �� ��� ���� ������, ����� ���� �� ����� */
	if ((_nCurrentThreadCount - _nFreeThreadCount) > _nBusyThreadCount &&
		!bAddAnyway)
		return false;

	_nCurrentThreadCount++;

	/** ��������� ����� ���� */
	try
	{
		if (isInitialize())
			CThreadPoolWorker::start(this);
		else
			return false;
	}
	catch (const std::exception& ex)
	{
		/** ���� �� ����� �������� ������� ���������� ����������
			�� ���������� �������� ���������� ����� ������� */
		_nCurrentThreadCount--;
		_pIocp->log(
			logger::EMessageType::critical, ex);

		throw;
	}

	return true;
}
//==============================================================================
void CThreadPoolPrefix::decrementBusyWorker() noexcept
{
	cs::CCriticalSectionScoped lock(_csCounter);
	_nBusyThreadCount--;
}
//==============================================================================
void CThreadPoolPrefix::release() noexcept
{
	{
		/** ��������� ������� ��������� ��� ������������� */
		cs::CCriticalSectionScoped lock(_csCounter);

		for (DWORD i = 0; i < _nCurrentThreadCount; i++)
		{
			/** �������� ������ ���������� */
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
					/** �������� �� ��������� ������ */
					_pIocp->log(
						logger::EMessageType::warning,
						ex);
				}
			}
		}
	}

	/** ������� ����� ��� */
	__super::release();
}
//==============================================================================
CThreadPoolPrefix::~CThreadPool()
{
	release();
}
//==============================================================================