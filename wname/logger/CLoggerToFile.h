#pragma once

_WNAME_BEGIN
namespace logger {
	/** ��� � ���� */
	class CLoggerToFile final : public ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* �����������.
		* @param logPath - ���� �� �����.
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
		* ����������.
		*/
		WNAME ~CLoggerToFile() = default;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* ���������� ���������.
		* @param wStr - ���������� ���������.
		*/
		WNAME void logWrite(
			const std::wstring& wStr) const override;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	//==========================================================================
	private:
		/** ���� �� ����� */
		const std::filesystem::path _logPath;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END