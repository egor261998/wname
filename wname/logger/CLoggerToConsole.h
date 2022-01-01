#pragma once

_WNAME_BEGIN
namespace logger {
	/** ��� � ������� */
	class CLoggerToConsole final : public ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* �����������.
		* @param logPath - ���� �� �����.
		*/
		WNAME CLoggerToConsole() noexcept;
	//==========================================================================
		CLoggerToConsole(const CLoggerToConsole&) = delete;
		CLoggerToConsole(CLoggerToConsole&&) = delete;
		CLoggerToConsole& operator=(const CLoggerToConsole&) = delete;
		CLoggerToConsole& operator=(CLoggerToConsole&&) = delete;
	//==========================================================================
		/**
		* ����������.
		*/
		WNAME ~CLoggerToConsole() = default;
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
			const std::wstring& wStr) const noexcept override;
	//==========================================================================
		#pragma endregion
	};
}
_WNAME_END