#pragma once

_WNAME_BEGIN
namespace logger {
	/** ��� � ���� */
	class WNAME CLoggerToFile final : public ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* �����������.
		* @param logPath - ���� �� �����.
		*/
		CLoggerToFile(
			const std::filesystem::path logPath);
	//==========================================================================
		/**
		* ����������.
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
		* ���������� ���������.
		* @param wStr - ���������� ���������.
		*/
		void logWrite(
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