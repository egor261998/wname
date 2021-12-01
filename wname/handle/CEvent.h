#pragma once

_WNAME_BEGIN
namespace handle
{
	/** реализация события в классе */
	class CEvent final : public CHandle
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		*/
		WNAME CEvent() noexcept = default;
	//==========================================================================
		/**
		* конструктор копирования.
		* @param handle - описатель копирования.
		*/
		WNAME CEvent(
			const CEvent& handle) noexcept;
	//==========================================================================
		/**
		* конструктор перемещения.
		* @param handle - описатель перемещения.
		*/
		WNAME CEvent(
			CEvent&& handle) noexcept;
	//==========================================================================
		/**
		* инициализировать событие.
		*/
		WNAME void initialize();
	//==========================================================================
		/**
		* дать уведомление.
		* @return - код ошибки.
		*/
		WNAME std::error_code notify() const noexcept;
	//==========================================================================
		template<class... Args>
		WNAME static std::error_code notify(
			const bool bWaitAll,
			const DWORD dwMilliseconds,
			Args&... args) 
		{
			std::vector<CEvent*> vecEvents({ &args... });
			std::vector<HANDLE> vecHandles;
			const auto numbers = vecEvents.size();
			vecHandles.resize(numbers);
			for (size_t i = 0; i < numbers; i++)
			{
				vecHandles.at(i) = vecEvents.at(i)->getHandle();
			}
			
			return std::error_code(
				WaitForMultipleObjects(
					static_cast<DWORD>(numbers), 
					vecHandles.data(), 
					bWaitAll, 
					dwMilliseconds),
				std::system_category());
		}
	//==========================================================================
		/**
		* ожидание уведомления.
		* @param dwMilliseconds - время ожидания уведомления.
		* @return - код ошибки.
		*/
		WNAME std::error_code wait(
			const DWORD dwMilliseconds = INFINITE) const noexcept;
	//==========================================================================
		/**
		* отменить уведомление.
		* @return - код ошибки.
		*/
		WNAME std::error_code cancel() const noexcept;
	//==========================================================================
		/**
		* оператор копирования описателя.
		* @param handle - копируемый объект.
		* @return - текущий объект.
		*/
		WNAME CEvent& operator=(
			const CEvent& handle) noexcept;
	//==========================================================================
		/**
		* оператор перемещения описателя.
		* @param handle - перемещаемый объект.
		* @return - текущий объект.
		*/
		WNAME CEvent& operator=(
			CEvent&& handle) noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		WNAME ~CEvent();
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END