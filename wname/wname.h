#pragma once

/** ������� */
#ifdef _WINDLL
#define WNAME __declspec(dllexport)
#else
#define WNAME 
#endif

/** ������������ ���� */
#define _WNAME_BEGIN namespace wname {
#define _WNAME_END }

/** ����������� ����������� */
#include <windows.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <cassert>
#include <thread>
#include <Shlwapi.h>

/** ����������� */
#include "logger\EMessageType.h"
#include "logger\Ilogger.h"
#include "logger\CLoggerToFile.h"

/** ����������� ������ */
#include "critical section\CCriticalSection.h"
#include "critical section\CCriticalSectionScoped.h"

/** ������� ���� */
#include "handle\CHandle.h"
#include "handle\CEvent.h"

/** ������ */
#include "misc\CThreadTls.h"
#include "misc\CInterlockedList.h"
#include "misc\CCounter.h"
#include "misc\CCounterScoped.h"

/** ����/����� */
#include "io\iocp\CIocp.h"
#include "io\iocp\CThreadPool.h"
#include "io\iocp\CThreadPoolWorker.h"
#include "io\iocp\CAsyncOperation.h"
#include "io\CAsyncIo.h"

/** �������������� � �������� �������� */
#include "filesystem\CFile.h"
#include "filesystem\CDirectory.h"