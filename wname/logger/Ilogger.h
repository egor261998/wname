#pragma once

_WNAME_BEGIN
namespace logger
{
	/** интерфейс логирования */
	class ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		*/
		WNAME ILogger() noexcept = default;
	//==========================================================================
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param ex - исключение.
		* @param ec - код ошибки.
		* @param szMessageFunction - функция из которой вызвалось сообщение.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const std::exception& ex,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param szMessage - сообщение.
		* @param ec - код ошибки.
		* @param szMessageFunction - функция из которой вызвалось сообщение.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const char* const szMessage,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param strMessage - сообщение.
		* @param ec - код ошибки.
		* @param szMessageFunction - функция из которой вызвалось сообщение.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const std::string strMessage,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param wszMessage - сообщение.
		* @param ec - код ошибки.
		* @param szMessageFunction - функция из которой вызвалось сообщение.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const wchar_t* const wszMessage,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param wStrMessage - сообщение.
		* @param ec - код ошибки.
		* @param szMessageFunction - функция из которой вызвалось сообщение.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const std::wstring wStrMessage,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME virtual ~ILogger() = default;
	//==========================================================================
		ILogger(const ILogger&) = delete;
		ILogger(ILogger&&) = delete;
		ILogger& operator=(const ILogger&) = delete;
		ILogger& operator=(ILogger&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/*
		* сборка сообщения.
		* @param eMessageType - тип сообщения.
		* @param wStrMessage - сообщение.
		* @param ec - код ошибки.
		* @param wStrFunction - функция из которой вызвалось сообщение.
		* @return - логируемое сообщение.
		*/
		virtual std::wstring assembleMessage(
			const EMessageType eMessageType,
			const std::wstring& wStrMessage,
			const std::error_code& ec,
			const std::wstring& wStrFunction) const;
	//==========================================================================
		/**
		* логировать сообщение.
		* @param wStr - логируемое сообщение.
		*/
		virtual void logWrite(
			const std::wstring& wStr) const = 0;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		static std::atomic_long _uRepeatLog;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END