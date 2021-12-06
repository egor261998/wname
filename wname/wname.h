#pragma once

/** экспорт */
#ifdef _WINDLL
#define WNAME __declspec(dllexport)
#else
#define WNAME 
#endif

/** пространство имен */
#define _WNAME_BEGIN namespace wname {
#define _WNAME_END }

/** стандартное подключение */
#include <windows.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <cassert>
#include <thread>
#include <Shlwapi.h>

/** логирование */
#include "logger\EMessageType.h"
#include "logger\Ilogger.h"
#include "logger\CLoggerToFile.h"

/** критическая секция */
#include "critical section\CCriticalSection.h"
#include "critical section\CCriticalSectionScoped.h"

/** объекты ядра */
#include "handle\CHandle.h"
#include "handle\CEvent.h"

/** разное */
#include "misc\CThreadTls.h"
#include "misc\CInterlockedList.h"
#include "misc\CCounter.h"
#include "misc\CCounterScoped.h"

/** ввод/вывод */
#include "io\iocp\CIocp.h"
#include "io\iocp\CThreadPool.h"
#include "io\iocp\CThreadPoolWorker.h"
#include "io\iocp\CAsyncOperation.h"
#include "io\CAsyncIo.h"

/** взаимодействие с файловой системой */
#include "filesystem\CFile.h"
#include "filesystem\CDirectory.h"