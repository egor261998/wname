#pragma once

_WNAME_BEGIN
namespace logger {
	/** лог в файл */
	class WNAME CLoggerToFile final : public ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* конструктор.
		* @param logPath - путь до файла.
		*/
		CLoggerToFile(
			const std::filesystem::path logPath);
	//==========================================================================
		/**
		* деструктор.
		*/
		~CLoggerToFile() = default;
	//==========================================================================
		CLoggerToFile(const CLoggerToFile&) = delete;
		CLoggerToFile(CLoggerToFile&&) = delete;
		CLoggerToFile& operator=(const CLoggerToFile&) = delete;
		CLoggerToFile& operator=(CLoggerToFile&&) = delete;	
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
			const std::wstring& wStr) const override;
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