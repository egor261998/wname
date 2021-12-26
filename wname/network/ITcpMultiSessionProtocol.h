#pragma once

_WNAME_BEGIN
namespace network
{
	/** интерфейс много сессионного сетевого обмена */
	class ITcpMultiSessionProtocol : public misc::CCounter
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** шапка передачи */
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
		/** буфер на чтение или запись */
		#pragma pack(push, 1)
		typedef struct _SBufferIo
		{
			/** шапка передачи */
			SProtocolHeader _header;

			/** размер буфера */
			DWORD _dwSize;
			/** буфер передачи */
			PBYTE _pBuffer;

			/** количество сессий участвующих в передачи */
			std::atomic_uint _dwCountSessionPart;
		}SBufferIo, * PSBufferIo;
		#pragma pack(pop)
	//==========================================================================
		/** информация о подключении */
		#pragma pack(push, 1)
		typedef struct _SConnectionInfo
		{
			/** шапка передачи */
			SProtocolHeader _header = { SProtocolHeader::EInfo::eConnectionInfo };

			/** минимальное количество сессий передачи */
			DWORD _dwMinSession = 0;
			/** максимальное количество сессий передачи */
			DWORD _dwMaxSession = 1;

			/** максимальная пропускная способность для всего подключения */
			DWORD _dwMaxByteCapacity = 1024*1024*1024;
		}SConnectionInfo, *PSConnectionInfo;
		#pragma pack(pop)
	//==========================================================================
		/** информация о конкретной сессии */
		#pragma pack(push, 1)
		typedef struct _SSessionInfo
		{
			/** шапка передачи */
			SProtocolHeader _header = { SProtocolHeader::EInfo::eSessionInfo };

			/** уникальный индификатор сессии */
			GUID _guid;
			
			/** запрос добавления новой сессии */
			BOOL _bIsAddSession;
			/** запрос удаления сессии */
			BOOL _bIsDelSession;
		}SSessionInfo, * PSSessionInfo;
		#pragma pack(pop)
	//==========================================================================
		/** информация о передаваемом буфере */
		#pragma pack(push, 1)
		typedef struct _SBufferInfo
		{
			/** контрольная сумма всего буфера */
			DWORD _dwCrc;
			/** размер буфера */
			DWORD _dwSize;

			/** количество фрагментированных частей */
			DWORD _dwSessionFragment;
		}SBufferInfo, * PSBufferInfo;
		#pragma pack(pop)
	//==========================================================================
		/** информация о части буфера */
		#pragma pack(push, 1)
		typedef struct _SBufferSessionInfo
		{
			/** уникальный индификатор сессии */
			GUID _guid;

			/** размер буфера */
			DWORD _dwSize;
			/** смещение буфера от начала */
			DWORD _dwOffset;
		}SBufferSessionInfo, * PSBufferSessionInfo;
		#pragma pack(pop)
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор много сессионного сетевого обмена.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		WNAME ITcpMultiSessionProtocol(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* виртуальное синхронное соединение.
		* @return - код ошибки.
		*/
		WNAME virtual std::error_code connect() noexcept = 0;
	//==========================================================================
		/**
		* виртуальный старт асинхронной записи.
		* @param bufferSend - буфер для записи.
		* @param dwBufferSize - размер буфера для записи.
		* @return - код ошибки.
		*/
		WNAME virtual std::error_code startAsyncSend(
			const PBYTE bufferSend,
			const DWORD dwBufferSize) = 0;
	//==========================================================================
		/**
		* виртуальный старт асинхронного чтения.
		* @param bufferRecv - буфер для записи.
		* @param dwBufferSize - размер буфера для записи.
		* @return - код ошибки.
		*/
		WNAME virtual std::error_code startAsyncRecv(
			const PBYTE bufferRecv,
			const DWORD dwBufferSize) = 0;
	//==========================================================================
		/**
		* виртуально разорвать соединение.
		* @param ec - код ошибки завершения.
		*/
		WNAME virtual void disconnect(
			const std::error_code ec = std::error_code()) noexcept = 0;
	//==========================================================================
		/**
		* деструктор.
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
		* виртуальный обработчик события завершения асинхронной записи.
		* @param bufferSend - буфер данных.
		* @param dwReturnSize - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME virtual void asyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронного чтения.
		* @param bufferRecv - буфер данных.
		* @param dwReturnSize - количество прочитанных байт.
		* @param ec - код ошибки завершения.
		*/
		WNAME virtual void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept = 0;
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронного чтения.
		* @param bufferRecv - буфер данных.
		* @param dwReturnSize - количество полученных байт.
		* @param ec - код завершения.
		*/
		WNAME virtual void clientAsyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события завершения асинхронной записи.
		* @param bufferSend - буфер данных.
		* @param dwReturnSize - количество переданных байт.
		* @param ec - код завершения.
		*/
		WNAME virtual void clientAsyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события подключения клиента.
		* @param ec - код ошибки.
		*/
		WNAME virtual void clientConnected(
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальный обработчик события отключения клиента.
		* @param ec - код ошибки.
		*/
		WNAME virtual void clientDisconnected(
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/* порт ввода/вывода */
		const std::shared_ptr<io::iocp::CIocp> _pIocp;

		/** состояние */
		socket::ESocketState _eSocketState = socket::ESocketState::disconnected;
		/** адрес сервера */
		std::string _strIp;
		/** порт сервера */
		std::string _wPort;

		/** информация о соединении */
		SConnectionInfo _connectionInfo;
		/** информация о соединении удаленного подключения */
		SConnectionInfo _connectionInfoRemote;

		/** очередь на прием */
		std::list<SBufferIo> _recvQueue;
		PSBufferIo _pRecvBuffer = nullptr;

		/** очередь на отправку */
		std::list<SBufferIo> _sendQueue;
		PSBufferIo _pSendBuffer = nullptr;

		/** ошибка */
		std::error_code _ec;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END