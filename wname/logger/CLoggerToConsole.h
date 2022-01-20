#pragma once

_WNAME_BEGIN
namespace logger {
	/** ��� � ������� */
	class WNAME CLoggerToConsole final : public ILogger
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/*
		* �����������.
		* @param logPath - ���� �� �����.
		*/
		CLoggerToConsole() noexcept = default;
	//==========================================================================
		/**
		* ����������.
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
		* ���������� ���������.
		* @param wStr - ���������� ���������.
		*/
		void logWrite(
			const std::wstring& wStr) const noexcept override;
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END