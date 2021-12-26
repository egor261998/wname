#pragma once

_WNAME_BEGIN
namespace network
{
	/** ��������� ����� ����������� �������� ������ */
	class ITcpMultiSessionProtocol : public misc::CCounter
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** ����� �������� */
		#pragma pack(push, 1)
		typedef struct _SProtocolHeader
		{
			enum class EInfo :UINT8
			{
				eConnectionInfo,
				eSessionInfo,
			};

			EInfo _eInfo;
		}SProtocolHeader, * PSProtocolHeader;
		#pragma pack(pop)
	//==========================================================================
		/** ����� �� ������ ��� ������ */
		#pragma pack(push, 1)
		typedef struct _SBufferIo
		{
			/** ����� �������� */
			SProtocolHeader _header;

			/** ������ ������ */
			DWORD _dwSize;
			/** ����� �������� */
			PBYTE _pBuffer;

			/** ���������� ������ ����������� � �������� */
			std::atomic_uint _dwCountSessionPart;
		}SBufferIo, * PSBufferIo;
		#pragma pack(pop)
	//==========================================================================
		/** ���������� � ����������� */
		#pragma pack(push, 1)
		typedef struct _SConnectionInfo
		{
			/** ����� �������� */
			SProtocolHeader _header = { SProtocolHeader::EInfo::eConnectionInfo };

			/** ����������� ���������� ������ �������� */
			DWORD _dwMinSession = 0;
			/** ������������ ���������� ������ �������� */
			DWORD _dwMaxSession = 1;

			/** ������������ ���������� ����������� ��� ����� ����������� */
			DWORD _dwMaxByteCapacity = 1024*1024*1024;
		}SConnectionInfo, *PSConnectionInfo;
		#pragma pack(pop)
	//==========================================================================
		/** ���������� � ���������� ������ */
		#pragma pack(push, 1)
		typedef struct _SSessionInfo
		{
			/** ����� �������� */
			SProtocolHeader _header = { SProtocolHeader::EInfo::eSessionInfo };

			/** ���������� ����������� ������ */
			GUID _guid;
			
			/** ������ ���������� ����� ������ */
			BOOL _bIsAddSession;
			/** ������ �������� ������ */
			BOOL _bIsDelSession;
		}SSessionInfo, * PSSessionInfo;
		#pragma pack(pop)
	//==========================================================================
		/** ���������� � ������������ ������ */
		#pragma pack(push, 1)
		typedef struct _SBufferInfo
		{
			/** ����������� ����� ����� ������ */
			DWORD _dwCrc;
			/** ������ ������ */
			DWORD _dwSize;

			/** ���������� ����������������� ������ */
			DWORD _dwSessionFragment;
		}SBufferInfo, * PSBufferInfo;
		#pragma pack(pop)
	//==========================================================================
		/** ���������� � ����� ������ */
		#pragma pack(push, 1)
		typedef struct _SBufferSessionInfo
		{
			/** ���������� ����������� ������ */
			GUID _guid;

			/** ������ ������ */
			DWORD _dwSize;
			/** �������� ������ �� ������ */
			DWORD _dwOffset;
		}SBufferSessionInfo, * PSBufferSessionInfo;
		#pragma pack(pop)
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ����� ����������� �������� ������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		WNAME ITcpMultiSessionProtocol(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ����������� ���������� ����������.
		* @return - ��� ������.
		*/
		WNAME virtual std::error_code connect() noexcept = 0;
	//==========================================================================
		/**
		* ����������� ����� ����������� ������.
		* @param bufferSend - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @return - ��� ������.
		*/
		WNAME virtual std::error_code startAsyncSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize) = 0;
	//==========================================================================
		/**
		* ����������� ����� ������������ ������.
		* @param bufferRecv - ����� ��� ������.
		* @param dwBufferSize - ������ ������ ��� ������.
		* @return - ��� ������.
		*/
		WNAME virtual std::error_code startAsyncRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize) = 0;
	//==========================================================================
		/**
		* ���������� ��������� ����������.
		* @param ec - ��� ������ ����������.
		*/
		WNAME virtual void disconnect(
			const std::error_code ec = std::error_code()) noexcept = 0;
	//==========================================================================
		/**
		* ����������.
		*/
		WNAME ~ITcpMultiSessionProtocol();
	//==========================================================================
		ITcpMultiSessionProtocol(const ITcpMultiSessionProtocol&) = delete;
		ITcpMultiSessionProtocol(ITcpMultiSessionProtocol&&) = delete;
		ITcpMultiSessionProtocol& operator=(const ITcpMultiSessionProtocol&) = delete;
		ITcpMultiSessionProtocol& operator=(ITcpMultiSessionProtocol&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Peotected_Method
	protected:
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ����������� ������.
		* @param bufferSend - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ������ ����������.
		*/
		WNAME virtual void asyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ������������ ������.
		* @param bufferRecv - ����� ������.
		* @param dwReturnSize - ���������� ����������� ����.
		* @param ec - ��� ������ ����������.
		*/
		WNAME virtual void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ������������ ������.
		* @param bufferRecv - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		WNAME virtual void clientAsyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� ����������� ������.
		* @param bufferSend - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		WNAME virtual void clientAsyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ����������� �������.
		* @param ec - ��� ������.
		*/
		WNAME virtual void clientConnected(
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ����������� ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		WNAME virtual void clientDisconnected(
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/* ���� �����/������ */
		const std::shared_ptr<io::iocp::CIocp> _pIocp;

		/** ��������� */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** ����� ������� */
		std::string _strIp;
		/** ���� ������� */
		std::string _wPort;

		/** ���������� � ���������� */
		SConnectionInfo _connectionInfo;
		/** ���������� � ���������� ���������� ����������� */
		SConnectionInfo _connectionInfoRemote;

		/** ������� �� ����� */
		std::list<SBufferIo> _recvQueue;
		PSBufferIo _pRecvBuffer = nullptr;

		/** ������� �� �������� */
		std::list<SBufferIo> _sendQueue;
		PSBufferIo _pSendBuffer = nullptr;

		/** ������ */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END