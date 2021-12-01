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

/** ����������� */
#include "logger\EMessageType.h"
#include "logger\Ilogger.h"
#include "logger\CLoggerToFile.h"

/** ������� ���� */
#include "handle\CHandle.h"
#include "handle\CEvent.h"

/** ������ */
#include "misc\CThreadTls.h"
#include "misc\CInterlockedList.h"
#include "misc\CCounter.h"
#include "misc\CCounterScoped.h"