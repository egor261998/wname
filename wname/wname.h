#pragma once

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")

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

/** ������������ */
#include "crypto\CMD5.h"
#include "crypto\CCrc32.h"

/** ����/����� */
#include "io\iocp\CIocp.h"
#include "io\iocp\CThreadPool.h"
#include "io\iocp\CThreadPoolWorker.h"
#include "io\iocp\CAsyncOperation.h"
#include "io\CAsyncIo.h"

/** �������������� � �������� �������� */
#include "filesystem\CFile.h"
#include "filesystem\CDirectory.h"

/** �������������� � ����� */
#include "network\socket\ESocketState.h"
#include "network\socket\CSocketHandle.h"
#include "network\socket\CSocketAdress.h"
#include "network\socket\CSocketIo.h"
#include "network\CTcpClient.h"
#include "network\CTcpServer.h"
#include "network\CTcpConnectedClient.h"

/** ����������� �������������� � ����� */
#include "network\ITcpMultiSessionProtocol.h"
#include "network\CTcpMultiSessionClient.h"