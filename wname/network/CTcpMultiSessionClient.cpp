#include "../stdafx.h"

using CTcpMiltiSessionClient = wname::network::CTcpMiltiSessionClient;
using ESocketStatePrefix = wname::network::socket::ESocketState;

//==============================================================================
CTcpMiltiSessionClient::CTcpMiltiSessionClient(
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<io::iocp::CIocp>& pIocp)
	:ITcpMultiSessionProtocol(strIp, wPort, pIocp)
{
	try
	{
		/** ������� �������� */
		initialize();

		/** ������� ������� ������ */
		_pTcpClientPartControl = std::make_unique<CTcpClientPart>(
			_strIp, _wPort, _pIocp, this);
	}
	catch (const std::exception& ex)
	{
		pIocp->log(logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTcpMiltiSessionClient::connect() noexcept
{
	misc::CCounterScoped counter(*this);

	if (!counter.isStartOperation())
		return std::error_code(ERROR_INVALID_HANDLE_STATE, std::system_category());

	std::error_code ec;

	{
		/** ��������� ������� ��������� ��� �������� ���������� */
		cs::CCriticalSectionScoped lock(_csCounter);

		if (_eSocketState == ESocketStatePrefix::disconnected)
		{
			/** ��������� */
			_eSocketState = ESocketStatePrefix::connecting;

			/** ��� ������ ������ ���� ��������� */
			if (!_transferSession.empty() ||
				!_recvQueue.empty() || _pRecvBuffer != nullptr ||
				!_sendQueue.empty() || _pSendBuffer != nullptr)
			{
				ec = std::error_code(WSAGetLastError(), std::system_category());
				_eSocketState = ESocketStatePrefix::disconnected;
			}
			else
			{
				ec = _pTcpClientPartControl->connect();
				if (ec)
				{
					/** �� ������� ����������� */
					_eSocketState = ESocketStatePrefix::disconnected;
				}
				else
				{
					/** ����� ���������� */
					_eSocketState = ESocketStatePrefix::connected;
				}			
			}
		}
		else
		{
			/** ��� ���� */
			ec = std::error_code(ERROR_INVALID_STATE, std::system_category());
		}
	}
	
	__super::clientConnected(ec);
	return ec;
}
//==============================================================================
std::error_code CTcpMiltiSessionClient::startAsyncSend(
	const PBYTE bufferSend,
	const DWORD dwBufferSize)
{

}
//==============================================================================
std::error_code CTcpMiltiSessionClient::startAsyncRecv(
	const PBYTE bufferRecv,
	const DWORD dwBufferSize)
{

}
//==============================================================================
void CTcpMiltiSessionClient::disconnect(
	const std::error_code ec) noexcept
{
	/** ������� �������� ��� ����������� ���������� ��������� */
	misc::CCounterScoped counter(*this);

	bool bIsRepeatDisconnect = false;
	bool bIsDisconnected = false;
	std::unordered_map<GUID, std::unique_ptr<CTcpClientPart>> transferSession;
	std::error_code ecDisconnected;

	{
		cs::CCriticalSectionScoped lock(_csCounter);

		switch (_eSocketState)
		{
		case ESocketStatePrefix::disconnected:
			break;
		case ESocketStatePrefix::disconnecting:
		{
			/** ��� �� ��� �������� ���������� */
			if (!_transferSession.empty() ||
				!_recvQueue.empty() || _pRecvBuffer != nullptr ||
				!_sendQueue.empty() || _pSendBuffer != nullptr)
			{
				if (!_transferSession.empty())
				{
					transferSession = std::move(_transferSession);
					bIsRepeatDisconnect = true;
				}

				break;
			}
				
			_eSocketState = ESocketStatePrefix::disconnected;
			ecDisconnected = _ec;
			bIsDisconnected = true;
			break;
		}
		case ESocketStatePrefix::connecting:
			break;
		case ESocketStatePrefix::connected:
		{
			_eSocketState = ESocketStatePrefix::disconnecting;
			_ec = ec;

			transferSession = std::move(_transferSession);
			bIsRepeatDisconnect = true;
			bIsRepeatDisconnect = true;
			break;
		}
		default:
			break;
		}
	}

	if (bIsRepeatDisconnect)
	{
		try
		{
			/** ����������� ��� ������ */
			for (auto it = transferSession.begin(); it != transferSession.end(); )
			{
				auto itCur = it;
				it++;
				const auto pSession = itCur->second.release();
				assert(pSession != nullptr);

				transferSession.erase(itCur);

				/** ������������ ������ */			
				pSession->disconnect();
				pSession->deleteAfterEndOperation();			
			}
		}
		catch (const std::exception& ex)
		{
			_pIocp->log(logger::EMessageType::warning, ex);
		}

		/** ��������� ���������� */
		disconnect();
	}

	if (bIsDisconnected)
	{
		/** ��������� ������� */
		__super::clientDisconnected(ecDisconnected);
	}
}
//==============================================================================
void CTcpMiltiSessionClient::asyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{

}
//==============================================================================
void CTcpMiltiSessionClient::asyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{

}
//==============================================================================
void CTcpMiltiSessionClient::clientConnected(
	CTcpClientPart* const pTcpClientPart,
	const std::error_code ec) noexcept
{
	assert(pTcpClientPart != nullptr);

	std::error_code ecClient;
	try
	{	
		while (true)
		{
			cs::CCriticalSectionScoped lock(_csCounter);
			if (pTcpClientPart == _pTcpClientPartControl.get())
			{
				/** ��������� ������������ ����������� */
				if (ec)
				{
					/** ����������� � ������� */
					ecClient = ec;
					break;
				}

				try
				{
					/** ������� ������� ���������� � ���������� */
					assert(_recvQueue.empty() && _pRecvBuffer);

					/** ������� ����� */
					_recvQueue.push_back(SBufferIo());
					_pRecvBuffer = &_recvQueue.back();
					_pRecvBuffer->_dwSize = sizeof(_connectionInfoRemote);
					_pRecvBuffer->_pBuffer = (PBYTE)&_connectionInfoRemote;
					ecClient = pTcpClientPart->startAsyncRecv(
						_pRecvBuffer->_pBuffer,
						_pRecvBuffer->_dwSize,
						MSG_WAITALL);
					if (ecClient)
					{
						_recvQueue.clear();
						_pRecvBuffer = nullptr;
						break;
					}
				}
				catch (const std::exception&)
				{
					/** ���-�� ����� �� ��� */
					_recvQueue.clear();
					_pRecvBuffer = nullptr;
					throw;
				}

				try
				{
					/** ������� ��������� ���������� � ���������� */
					assert(_sendQueue.empty() && _pSendBuffer);

					/** ������� ����� */
					_sendQueue.push_back(SBufferIo());
					_pSendBuffer = &_sendQueue.back();
					_pSendBuffer->_dwSize = sizeof(_connectionInfo);
					_pSendBuffer->_pBuffer = (PBYTE)&_connectionInfo;
					ecClient = pTcpClientPart->startAsyncSend(
						_pRecvBuffer->_pBuffer,
						_pRecvBuffer->_dwSize);
					if (ecClient)
					{
						_recvQueue.clear();
						_pRecvBuffer = nullptr;
						break;
					}
				}
				catch (const std::exception&)
				{
					/** ���-�� ����� �� ��� */
					_recvQueue.clear();
					_pRecvBuffer = nullptr;
					throw;
				}
			}
			else
			{
				/** ��������� ������������� ���������� �� ����� ���-�� ��������� */
			}
		}	
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::EMessageType::warning, ex);
		ecClient = std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
	}

	if (ecClient)
	{
		/** ����������� */
		disconnect(ecClient);
	}
}
//==============================================================================
void CTcpMiltiSessionClient::clientAsyncRecvComplitionHandler(
	CTcpClientPart* const pTcpClientPart,
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{

}
//==============================================================================
void CTcpMiltiSessionClient::clientAsyncSendComplitionHandler(
	CTcpClientPart* const pTcpClientPart,
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{

}
//==============================================================================
void CTcpMiltiSessionClient::clientDisconnected(
	CTcpClientPart* const pTcpClientPart,
	const std::error_code ec) noexcept
{
	assert(pTcpClientPart != nullptr);

	cs::CCriticalSectionScoped lock(_csCounter);
}
//==============================================================================
CTcpMiltiSessionClient::~CTcpMiltiSessionClient()
{
	/** ��������� */
	disconnect();

	/** ���� ���������� ����� */
	release();
}
//==============================================================================