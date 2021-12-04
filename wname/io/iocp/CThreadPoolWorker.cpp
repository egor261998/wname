#include "../../stdafx.h"

/** ���������� ����� ������������, �� ����� ��������� */
#define CThreadPoolWorkerPrefix wname::io::iocp::CIocp::CThreadPoolWorker

//==============================================================================
	CThreadPoolWorkerPrefix::CThreadPoolWorker(
		CThreadPool* const pThreadPool)
		:_pThreadPool(pThreadPool)
	{
		if (_pThreadPool == nullptr)
		{
			throw std::invalid_argument("pThreadPool == nullptr");
		}

		try
		{
			if (!_pThreadPool->startOperation())
			{
				throw std::logic_error("Thread pool is not start");
			}

			/** �������������� ������� */
			initialize();
		}
		catch (const std::exception& ex)
		{
			_pThreadPool->_pIocp->log(
				logger::EMessageType::warning, ex);

			throw;
		}
	}
//==============================================================================
	void CThreadPoolWorkerPrefix::start(
		CThreadPool* const pThreadPool)
	{
		if (pThreadPool == nullptr)
		{
			throw std::invalid_argument("pThreadPool == nullptr");
		}

		try
		{
			auto th = std::thread(
				&workerThread,
				pThreadPool->_pIocp,
				std::make_shared<CThreadPoolWorker>(pThreadPool));

			try
			{
				th.detach();
			}
			catch (const std::exception& ex)
			{
				pThreadPool->_pIocp->log(
					logger::EMessageType::warning, ex);
			}
		}
		catch (const std::exception& ex)
		{
			pThreadPool->_pIocp->log(
				logger::EMessageType::critical, ex);

			throw;
		}
	}
//==============================================================================
	void CThreadPoolWorkerPrefix::release() noexcept
	{
		/** ����� ���� �������� ����� ��� */
		__super::release();
	}
//==============================================================================
	CThreadPoolWorkerPrefix::~CThreadPoolWorker()
	{
		/** ���� ���������� */
		release();

		_pThreadPool->endOperation();
	}
//==============================================================================