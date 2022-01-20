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
		/** инициализируем порт завершени€ */
		_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		if (!_hIocp.isValid())
		{
			throw std::runtime_error("CreateIoCompletionPort failed with error: " +
				std::to_string(GetLastError()));
		}

		/** инициализируем пул потоков */
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
			/** дескриптор имеет не правильное значение */
			throw std::logic_error("hHandle = INVALID_HANDLE_VALUE || hHandle == nullptr");
		}

		auto hIocpRet = CreateIoCompletionPort(
			hHandle, _hIocp, ulCompletionKey, 0);

		if (hIocpRet != _hIocp)
		{
			/** не смогли прив€зать */
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
			/** создаем новую асинхронную операцию */
			pAsyncOperationResult =
				std::make_unique<CAsyncOperation>(
					this, pCompletionRoutineContext, fComplitionRoutine);
		}
		else
		{
			/** проверка контекста асинхронной операции */
			if (pAsyncOperationResult->_pIocp != this)
			{
				/** повреждена асинхронна€ операци€ */
				throw std::logic_error("pAsyncOperationResult->pIocp != this");
			}

			/** есть свободна€ асинхронна€ операци€ */
			pAsyncOperationResult->_pCompletionRoutineContext = pCompletionRoutineContext;
			pAsyncOperationResult->_fComplitionRoutine = fComplitionRoutine;
		}

		const auto freeOperationCount = _listFreeAsyncOperation.size();
		if ((_nMinReservAsyncOperation > freeOperationCount ||
			freeOperationCount > _nMaxReservAsyncOperation) &&
			!_updateOperation.exchange(true))
		{
			/** посылаем запрос пересчета асинхронных операций */
			if (!PostQueuedCompletionStatus(
				_hIocp,
				0,
				(ULONG_PTR)&_listFreeAsyncOperation,
				NULL))
			{
				/** не смогли так не смогли */
				_updateOperation = false;
				log(logger::EMessageType::warning,
					L"PostQueuedCompletionStatus failed with error: ",
					std::error_code(GetLastError(), std::system_category()));
			}
		}

		/** если нить пула, то вернетс€ контекст нити */
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

		/** посылаем транзитный запрос */
		if (!PostQueuedCompletionStatus(
			_hIocp,
			0,
			ulComplitionKey,
			&pAsyncOperation->_overlapped))
		{
			/** проблема транзита */
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
	/** завершаем работу */
	__super::release(false);

	/** ждем завершени€ всех потоков */
	_pThreadPool->release(true);

	/** закрываем порт ввода/вывода */
	_hIocp.close();

	/** ожидаем*/
	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//==============================================================================
CIocp::~CIocp()
{
	/** завершаем работу */
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
		/** код ошибки асинхронной операции */
		DWORD dwError = ERROR_SUCCESS;

		/** объект асинхронной операции */
		LPOVERLAPPED pOverlapped = nullptr;

		/** количество задействованных байт */
		DWORD dwBytesTransferred = 0;

		/** ключ асинхронной операции */
		ULONG_PTR ulCompletionKey = 0;

		/** ожидани€ завершени€ асинхронной операции */
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

					/** нас долго не используют, может уволитьс€? */
					if (pWorkerThread->_pThreadPool->canFreeWorker(pWorkerThread.get()))
						break;

					/** будет сложно но мы справимс€ */
					continue;
				}

				/** свалил Iocp, разбегаемс€ */
				log(logger::EMessageType::critical,
					L"IOCP Closed",
					std::error_code(dwError, std::system_category()));

				break;
			}

			/** работа была выполнена, но ошибка в “«.
			об это необходимо сообщить тому кто выдал “« */
		}

		/** особа€ проверка на закрытие Iocp */
		if (!pOverlapped &&
			ulCompletionKey == (ULONG_PTR)_pThreadPool.get())
			break;

		/** пора немного поработать
			может нан€ть еще одного работника? */
		pWorkerThread->_bIsBusy = true;

		try
		{
			pWorkerThread->_pThreadPool->canAddWorker();
		}
		catch (const std::exception& ex)
		{
			log(logger::EMessageType::critical, ex);
		}

		/**  особа€ проверка всех асинхронных операций */
		if (!pOverlapped &&
			ulCompletionKey == (ULONG_PTR)&_listFreeAsyncOperation)
		{
			/** используем обработчик исключений,
				чтобы не похоронить нить */
			try
			{
				/** удал€ем все лишние операции */
				for (size_t i = _nMinReservAsyncOperation;
					i < _listFreeAsyncOperation.size();
					i++)
					_listFreeAsyncOperation.popEx();

				/** добавл€ем операции */
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
			/** получаем контекст асинхронной операции */
			auto pOperation =
				CONTAINING_RECORD(pOverlapped, CAsyncOperation, _overlapped);

			/** провер€ем что контекст именно нашей компании,
			сравним его с работодателем */
			if (pOperation->_pIocp != this)
			{
				/** наша компани€ такое не обслуживает */
				log(logger::EMessageType::warning, L"pOperation->_pIocp != this");
			}
			else
			{
				/** заполн€ем отчет */
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

		/** мы снова готовы приступить к работе */
		pWorkerThread->_bIsBusy = false;
			
		_pThreadPool->decrementBusyWorker();
	}

	/** увольн€емс€ */
	pWorkerThread->_pThreadPool->delWorker(pWorkerThread.get());
}
//==============================================================================