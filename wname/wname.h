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

/** логирование */
#include "logger\EMessageType.h"
#include "logger\Ilogger.h"
#include "logger\CLoggerToFile.h"