#include "../stdafx.h"

using ITcpMultiSessionProtocolPrefix = wname::network::ITcpMultiSessionProtocol;

//==============================================================================
ITcpMultiSessionProtocolPrefix::ITcpMultiSessionProtocol(
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) 
	:_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		throw std::invalid_argument("_pIocp == nullptr");
	}

	try
	{
		_strIp = strIp;
		_wPort = wPort;

		/** инициализируем счетчик операций */
		initialize();
	}
	catch (const std::exception& ex)
	{
		pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void ITcpMultiSessionProtocolPrefix::clientAsyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferRecv);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void ITcpMultiSessionProtocolPrefix::clientAsyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferSend);
	UNREFERENCED_PARAMETER(dwReturnSize);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void ITcpMultiSessionProtocolPrefix::clientConnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void ITcpMultiSessionProtocolPrefix::clientDisconnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
ITcpMultiSessionProtocolPrefix::~ITcpMultiSessionProtocol()
{
	/** ждем завершения всего */
	release();
}
//==============================================================================