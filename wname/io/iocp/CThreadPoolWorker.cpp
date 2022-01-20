#include "../../stdafx.h"

/** объявление через препроцессор, тк класс приватный */
using wname::io::iocp::CIocp;

//==============================================================================
CIocp::CThreadPoolWorker::CThreadPoolWorker(
	CThreadPool* const pThreadPool)	:
	_pThreadPool(pThreadPool)
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
	}
	catch (const std::exception& ex)
	{
		_pThreadPool->_pIocp->log(
			logger::EMessageType::warning, ex);

		throw;
	}
}
//==============================================================================
void CIocp::CThreadPoolWorker::start(
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
void CIocp::CThreadPoolWorker::release(
	const bool bIsWait) noexcept
{
	/** ждать всех операций нужно тут */
	__super::release(bIsWait);
}
//==============================================================================
CIocp::CThreadPoolWorker::~CThreadPoolWorker()
{
	/** ждем завершения */
	release(true);

	_pThreadPool->endOperation();
}
//==============================================================================