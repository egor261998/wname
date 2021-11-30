#pragma once

_WNAME_BEGIN
namespace logger {
	/** лог в файл */
	class CLoggerToFile : public ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* конструктор из std::wstring.
		* @param logPath - путь до файла.
		*/
		WNAME CLoggerToFile(
			const std::filesystem::path logPath);
	//==========================================================================
		CLoggerToFile(const CLoggerToFile&) = delete;
		CLoggerToFile(CLoggerToFile&&) = delete;
		CLoggerToFile& operator=(const CLoggerToFile&) = delete;
		CLoggerToFile& operator=(CLoggerToFile&&) = delete;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CLoggerToFile() = default;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* логировать сообщение.
		* @param str - логируемое сообщение.
		*/
		void logWrite(
			const std::wstring& str) const override;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	//==========================================================================
	private:
		/** путь до файла */
		const std::filesystem::path _logPath;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END