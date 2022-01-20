#pragma once

_WNAME_BEGIN
namespace handle
{
	/** реализация события в классе */
	class WNAME CEvent final : public CHandle
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор по умолчанию.
		*/
		CEvent() noexcept(false);
	//==========================================================================
		/**
		* конструктор копирования.
		* @param handle - описатель копирования.
		*/
		CEvent(
			const CEvent& handle) noexcept;
	//==========================================================================
		/**
		* конструктор перемещения.
		* @param handle - описатель перемещения.
		*/
		CEvent(
			CEvent&& handle) noexcept;
	//==========================================================================
		/**
		* дать уведомление.
		* @return - код ошибки.
		*/
		std::error_code notify() const noexcept;
	//==========================================================================
		/**
		* ожидание уведомлений для нескольких событий.
		* @param bWaitAll - ожидать всех.
		* @param dwMilliseconds - время ожидания уведомления.
		* @param args - события.
		* @return - код ошибки.
		*/
		template<class... Args>
		static std::error_code wait(
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
		std::error_code wait(
			const DWORD dwMilliseconds = INFINITE) const noexcept;
	//==========================================================================
		/**
		* отменить уведомление.
		* @return - код ошибки.
		*/
		std::error_code cancel() const noexcept;
	//==========================================================================
		/**
		* оператор копирования описателя.
		* @param handle - копируемый объект.
		* @return - текущий объект.
		*/
		CEvent& operator=(
			const CEvent& handle) noexcept;
	//==========================================================================
		/**
		* оператор перемещения описателя.
		* @param handle - перемещаемый объект.
		* @return - текущий объект.
		*/
		CEvent& operator=(
			CEvent&& handle) noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		~CEvent();
	//==========================================================================
	#pragma endregion
	};
}
_WNAME_END