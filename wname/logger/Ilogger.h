#pragma once

_WNAME_BEGIN
namespace logger
{
	/** ��������� ����������� */
	class ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �� ���������.
		*/
		WNAME ILogger() noexcept = default;
	//==========================================================================
		/*
		* ��� ���������.
		* @param eMessageType - ��� ���������.
		* @param ex - ����������.
		* @param ec - ��� ������.
		* @param szMessageFunction - ������� �� ������� ��������� ���������.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const std::exception& ex,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* ��� ���������.
		* @param eMessageType - ��� ���������.
		* @param szMessage - ���������.
		* @param ec - ��� ������.
		* @param szMessageFunction - ������� �� ������� ��������� ���������.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const char* const szMessage,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* ��� ���������.
		* @param eMessageType - ��� ���������.
		* @param strMessage - ���������.
		* @param ec - ��� ������.
		* @param szMessageFunction - ������� �� ������� ��������� ���������.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const std::string strMessage,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* ��� ���������.
		* @param eMessageType - ��� ���������.
		* @param wszMessage - ���������.
		* @param ec - ��� ������.
		* @param szMessageFunction - ������� �� ������� ��������� ���������.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const wchar_t* const wszMessage,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/*
		* ��� ���������.
		* @param eMessageType - ��� ���������.
		* @param wStrMessage - ���������.
		* @param ec - ��� ������.
		* @param szMessageFunction - ������� �� ������� ��������� ���������.
		*/
		WNAME void log(
			const EMessageType eMessageType,
			const std::wstring wStrMessage,
			const std::error_code ec = std::error_code(),
			const char* const szMessageFunction = __builtin_FUNCTION()) const noexcept;
	//==========================================================================
		/**
		* ����������.
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
		* ������ ���������.
		* @param eMessageType - ��� ���������.
		* @param wStrMessage - ���������.
		* @param ec - ��� ������.
		* @param wStrFunction - ������� �� ������� ��������� ���������.
		* @return - ���������� ���������.
		*/
		virtual std::wstring assembleMessage(
			const EMessageType eMessageType,
			const std::wstring& wStrMessage,
			const std::error_code& ec,
			const std::wstring& wStrFunction) const;
	//==========================================================================
		/**
		* ���������� ���������.
		* @param wStr - ���������� ���������.
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