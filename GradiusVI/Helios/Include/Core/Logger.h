#pragma once
#pragma warning( push )
#pragma warning ( disable : 26495 )
#pragma warning ( disable : 26439 )
#pragma warning ( disable : 26451 )
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/base_sink.h>
#pragma warning( pop )

#include "Editor/GUILogger.h"
#include <mutex>

namespace Helios
{
	class Logger
	{
	public:
		static void Initialize();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_clientLogger; }
		static void SetupGUILogger(std::vector<std::pair<GUILogger::ELevelType, std::string>>& a_logger);
		static void RemoveGUILogger();
	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;

		static std::shared_ptr<GUILogger> s_guiLogger;

		static bool s_GUILoggerInitialized;
	};

	class GUISink : public spdlog::sinks::base_sink<std::mutex>
	{
	public:
		GUISink(const std::shared_ptr<GUILogger>& a_guiLogger);
	protected:
		void sink_it_(const spdlog::details::log_msg&) override;

		void flush_() override;

	private:
		std::shared_ptr<GUILogger> m_guiLogger;
	};
}


#if HE_DEBUG
//Core logging templates
template<typename... T>
constexpr void HE_CORE_TRACE(T&& ... args) { Helios::Logger::GetCoreLogger()->trace(std::forward<T>(args)...); }
template<typename... T>
constexpr void HE_CORE_INFO(T&& ... args) { Helios::Logger::GetCoreLogger()->info(std::forward<T>(args)...); }
template<typename... T>
constexpr void HE_CORE_WARN(T&& ... args) { Helios::Logger::GetCoreLogger()->warn(std::forward<T>(args)...); }
template<typename... T>
constexpr void HE_CORE_ERROR(T&& ... args) { Helios::Logger::GetCoreLogger()->error(std::forward<T>(args)...); }
template<typename... T>
constexpr void HE_CORE_CRITICAL(T&& ... args) { Helios::Logger::GetCoreLogger()->critical(std::forward<T>(args)...); }

//Client/Game logging templates
template<typename... T>
constexpr void LOG_TRACE(T&& ... args) { Helios::Logger::GetClientLogger()->trace(std::forward<T>(args)...); }
template<typename... T>
constexpr void LOG_INFO(T&& ... args) { Helios::Logger::GetClientLogger()->info(std::forward<T>(args)...); }
template<typename... T>
constexpr void LOG_WARN(T&& ... args) { Helios::Logger::GetClientLogger()->warn(std::forward<T>(args)...); }
template<typename... T>
constexpr void LOG_ERROR(T&& ... args) { Helios::Logger::GetClientLogger()->error(std::forward<T>(args)...); }
template<typename... T>
constexpr void LOG_CRITICAL(T&& ... args) { Helios::Logger::GetClientLogger()->critical(std::forward<T>(args)...); }

#else
#define HE_CORE_TRACE
#define HE_CORE_INFO
#define HE_CORE_WARN
#define HE_CORE_ERROR
#define HE_CORE_CRITICAL

#define LOG_TRACE
#define LOG_INFO
#define LOG_WARN
#define LOG_ERROR
#define LOG_CRITICAL
#endif