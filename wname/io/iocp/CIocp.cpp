#include "../../stdafx.h"

using wname::io::iocp::CIocp;

//==============================================================================
CIocp::CIocp(
	const std::shared_ptr<logger::ILogger> pLogger) noexcept(false)	:
	CIocp(1, std::thread::hardware_concurrency(), pLogger)
{
	
}
//==============================================================================
CIocp::CIocp(
	const DWORD minThreadCount,
	const DWORD maxThreadCount,
	const std::shared_ptr<logger::ILogger> pLogger) noexcept(false)
{
	_pLogger = pLogger;

	try
	{
		/** �������������� ���� ���������� */
		_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		if (!_hIocp.isValid())
		{
			throw std::runtime_error("CreateIoCompletionPort failed with error: " +
				std::to_string(GetLastError()));
		}

		/** �������������� ��� ������� */
		_pThreadPool = std::make_unique<CThreadPool>
			(this, minThreadCount, maxThreadCount);
	}
	catch (const std::exception& ex)
	{
		log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CIocp::bind(
	const HANDLE hHandle,
	const ULONG_PTR ulCompletionKey)
{
	misc::CCounterScoped counter(*this);

	try
	{
		if (!counter.isStartOperation())
		{
			throw std::logic_error("IOCP is not start");
		}

		if (hHandle == INVALID_HANDLE_VALUE || hHandle == nullptr)
		{
			/** ���������� ����� �� ���������� �������� */
			throw std::logic_error("hHandle = INVALID_HANDLE_VALUE || hHandle == nullptr");
		}

		auto hIocpRet = CreateIoCompletionPort(
			hHandle, _hIocp, ulCompletionKey, 0);

		if (hIocpRet != _hIocp)
		{
			/** �� ������ ��������� */
			throw std::runtime_error("Bind Iocp failed with error: " +
				std::to_string(GetLastError()));
		}
	}
	catch (const std::exception& ex)
	{
		log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
CIocp::CAsyncOperation* CIocp::getAsyncOperation(
	const PVOID pCompletionRoutineContext,
	const FAsyncCompletion fComplitionRoutine)
{
	misc::CCounterScoped counter(*this);

	try
	{
		if (!counter.isStartOperation())
		{
			throw std::logic_error("IOCP is not start");
		}

		auto pAsyncOperationResult = _listFreeAsyncOperation.popEx();
		if (pAsyncOperationResult == nullptr)
		{
			/** ������� ����� ����������� �������� */
			pAsyncOperationResult =
				std::make_unique<CAsyncOperation>(
					this, pCompletionRoutineContext, fComplitionRoutine);
		}
		else
		{
			/** �������� ��������� ����������� �������� */
			if (pAsyncOperationResult->_pIocp != this)
			{
				/** ���������� ����������� �������� */
				throw std::logic_error("pAsyncOperationResult->pIocp != this");
			}

			/** ���� ��������� ����������� �������� */
			pAsyncOperationResult->_pCompletionRoutineContext = pCompletionRoutineContext;
			pAsyncOperationResult->_fComplitionRoutine = fComplitionRoutine;
		}

		const auto freeOperationCount = _listFreeAsyncOperation.size();
		if ((_nMinReservAsyncOperation > freeOperationCount ||
			freeOperationCount > _nMaxReservAsyncOperation) &&
			!_updateOperation.exchange(true))
		{
			/** �������� ������ ��������� ����������� �������� */
			if (!PostQueuedCompletionStatus(
				_hIocp,
				0,
				(ULONG_PTR)&_listFreeAsyncOperation,
				NULL))
			{
				/** �� ������ ��� �� ������ */
				_updateOperation = false;
				log(logger::EMessageType::warning,
					L"PostQueuedCompletionStatus failed with error: ",
					std::error_code(GetLastError(), std::system_category()));
			}
		}

		/** ���� ���� ����, �� �������� �������� ���� */
		pAsyncOperationResult->_pThreadPoolWorker = _tls.getValue();
		if (pAsyncOperationResult->_pThreadPoolWorker != nullptr)
		{
			if (!pAsyncOperationResult->_pThreadPoolWorker->startOperation())
			{
				throw std::logic_error("!pAsyncOperationResult->_pThreadPoolWorker->startOperation()");
			}
		}
		ZeroMemory(&pAsyncOperationResult->_overlapped, sizeof(OVERLAPPED));
		counter.release();
		return pAsyncOperationResult.release();
	}
	catch (const std::exception& ex)
	{
		log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CIocp::transit(
	const PVOID pCompletionRoutineContext,
	const FAsyncCompletion fComplitionRoutine,
	const ULONG_PTR ulComplitionKey)
{
	misc::CCounterScoped counter(*this);

	try
	{
		if (!counter.isStartOperation())
		{
			throw std::logic_error("IOCP is not start");
		}

		auto pAsyncOperation = getAsyncOperation(
			pCompletionRoutineContext, fComplitionRoutine);
		if (pAsyncOperation == nullptr)
		{
			throw std::invalid_argument("pAsyncOperation == nullptr");
		}

		pAsyncOperation->_pCompletionRoutineContext = pCompletionRoutineContext;
		pAsyncOperation->_fComplitionRoutine = fComplitionRoutine;

		/** �������� ���������� ������ */
		if (!PostQueuedCompletionStatus(
			_hIocp,
			0,
			ulComplitionKey,
			&pAsyncOperation->_overlapped))
		{
			/** �������� �������� */
			pAsyncOperation->cancel();

			throw std::runtime_error("PostQueuedCompletionStatus failed with error: " +
				std::to_string(GetLastError()));
		}
	}
	catch (const std::exception& ex)
	{
		log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CIocp::log(
	const logger::EMessageType eMessageType,
	const std::exception& ex,
	const std::error_code ec,
	const char* const messageFunction) const noexcept
{
	if (_pLogger != nullptr)
		_pLogger->log(eMessageType, ex, ec, messageFunction);
}
//==============================================================================
void CIocp::log(
	const logger::EMessageType eMessageType,
	const WCHAR* const message,
	const std::error_code ec,
	const char* const messageFunction) const noexcept
{
	if (_pLogger != nullptr)
		_pLogger->log(eMessageType, message, ec, messageFunction);
}
//==============================================================================
CIocp::operator HANDLE() noexcept
{
	return _hIocp;
}
//==============================================================================
void CIocp::release(
	const bool bIsWait) noexcept
{
	/** ��������� ������ */
	__super::release(false);

	/** ���� ���������� ���� ������� */
	_pThreadPool->release(true);

	/** ��������� ���� �����/������ */
	_hIocp.close();

	/** �������*/
	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//==============================================================================
CIocp::~CIocp()
{
	/** ��������� ������ */
	release(true);
}
//==============================================================================
void CIocp::workerThread(
	const std::shared_ptr<CThreadPoolWorker> pWorkerThread) noexcept
{
	#pragma warning (disable: 26493 26481)
	assert(pWorkerThread != nullptr);

	while (true)
	{
		/** ��� ������ ����������� �������� */
		DWORD dwError = ERROR_SUCCESS;

		/** ������ ����������� �������� */
		LPOVERLAPPED pOverlapped = nullptr;

		/** ���������� ��������������� ���� */
		DWORD dwBytesTransferred = 0;

		/** ���� ����������� �������� */
		ULONG_PTR ulCompletionKey = 0;

		/** �������� ���������� ����������� �������� */
		if (!GetQueuedCompletionStatus(
			_hIocp,
			&dwBytesTransferred, &ulCompletionKey,
			&pOverlapped,
			_nWorkerTimeIde))
		{
			dwError = GetLastError();

			if (!pOverlapped)
			{
				if (dwError == WAIT_TIMEOUT)
				{

					/** ��� ����� �� ����������, ����� ���������? */
					if (pWorkerThread->_pThreadPool->canFreeWorker(pWorkerThread.get()))
						break;

					/** ����� ������ �� �� ��������� */
					continue;
				}

				/** ������ Iocp, ����������� */
				log(logger::EMessageType::critical,
					L"IOCP Closed",
					std::error_code(dwError, std::system_category()));

				break;
			}

			/** ������ ���� ���������, �� ������ � ��.
			�� ��� ���������� �������� ���� ��� ����� �� */
		}

		/** ������ �������� �� �������� Iocp */
		if (!pOverlapped &&
			ulCompletionKey == (ULONG_PTR)_pThreadPool.get())
			break;

		/** ���� ������� ����������
			����� ������ ��� ������ ���������? */
		pWorkerThread->_bIsBusy = true;

		try
		{
			pWorkerThread->_pThreadPool->canAddWorker();
		}
		catch (const std::exception& ex)
		{
			log(logger::EMessageType::critical, ex);
		}

		/**  ������ �������� ���� ����������� �������� */
		if (!pOverlapped &&
			ulCompletionKey == (ULONG_PTR)&_listFreeAsyncOperation)
		{
			/** ���������� ���������� ����������,
				����� �� ���������� ���� */
			try
			{
				/** ������� ��� ������ �������� */
				for (size_t i = _nMinReservAsyncOperation;
					i < _listFreeAsyncOperation.size();
					i++)
					_listFreeAsyncOperation.popEx();

				/** ��������� �������� */
				for (size_t i = _listFreeAsyncOperation.size();
					i < _nMaxReservAsyncOperation;
					i++)
					_listFreeAsyncOperation.pushEx(
						std::make_unique<CAsyncOperation>(this));
			}
			catch (const std::exception& ex)
			{
				log(logger::EMessageType::warning, ex);
			}

			_updateOperation = false;
		}
		else if (pOverlapped)
		{
			/** �������� �������� ����������� �������� */
			auto pOperation =
				CONTAINING_RECORD(pOverlapped, CAsyncOperation, _overlapped);

			/** ��������� ��� �������� ������ ����� ��������,
			������� ��� � ������������� */
			if (pOperation->_pIocp != this)
			{
				/** ���� �������� ����� �� ����������� */
				log(logger::EMessageType::warning, L"pOperation->_pIocp != this");
			}
			else
			{
				/** ��������� ����� */
				pOperation->_ec = std::error_code(dwError, std::system_category());
				pOperation->_dwReturnSize = dwBytesTransferred;
				pOperation->_ulCompletionKey = ulCompletionKey;

				try
				{
					if (pOperation->_fComplitionRoutine != nullptr)
						pOperation->_fComplitionRoutine(pOperation);
				}
				catch (const std::exception& ex)
				{
					log(logger::EMessageType::warning, ex);
				}

				if (pOperation->_pThreadPoolWorker != nullptr)
				{
					pOperation->_pThreadPoolWorker->endOperation();
				}

				_listFreeAsyncOperation.push(pOperation);
				endOperation();
			}
		}
		else
		{
			log(logger::EMessageType::warning, L"pOverlapped == nullptr");
		}

		/** �� ����� ������ ���������� � ������ */
		pWorkerThread->_bIsBusy = false;
			
		_pThreadPool->decrementBusyWorker();
	}

	/** ����������� */
	pWorkerThread->_pThreadPool->delWorker(pWorkerThread.get());
}
//==============================================================================