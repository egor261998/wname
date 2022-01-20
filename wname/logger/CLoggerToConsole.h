#pragma once

_WNAME_BEGIN
namespace logger {
	/** лог в консоль */
	class WNAME CLoggerToConsole final : public ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* конструктор.
		* @param logPath - путь до файла.
		*/
		CLoggerToConsole() noexcept = default;
	//==========================================================================
		/**
		* деструктор.
		*/
		~CLoggerToConsole() = default;
	//==========================================================================
		CLoggerToConsole(const CLoggerToConsole&) = delete;
		CLoggerToConsole(CLoggerToConsole&&) = delete;
		CLoggerToConsole& operator=(const CLoggerToConsole&) = delete;
		CLoggerToConsole& operator=(CLoggerToConsole&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* логировать сообщение.
		* @param wStr - логируемое сообщение.
		*/
		void logWrite(
			const std::wstring& wStr) const noexcept override;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END