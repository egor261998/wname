#pragma once

_WNAME_BEGIN
namespace logger {
	/** лог в консоль */
	class CLoggerToConsole final : public ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* конструктор.
		* @param logPath - путь до файла.
		*/
		WNAME CLoggerToConsole() noexcept;
	//==========================================================================
		CLoggerToConsole(const CLoggerToConsole&) = delete;
		CLoggerToConsole(CLoggerToConsole&&) = delete;
		CLoggerToConsole& operator=(const CLoggerToConsole&) = delete;
		CLoggerToConsole& operator=(CLoggerToConsole&&) = delete;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CLoggerToConsole() = default;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* логировать сообщение.
		* @param wStr - логируемое сообщение.
		*/
		WNAME void logWrite(
			const std::wstring& wStr) const noexcept override;
	//==========================================================================
		#pragma endregion
	};
}
_WNAME_END