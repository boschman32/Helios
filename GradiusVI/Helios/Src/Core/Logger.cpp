#include "hepch.h"
#include "Core/Logger.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Helios
{
	std::shared_ptr<spdlog::logger> Logger::s_coreLogger = nullptr;
	std::shared_ptr<spdlog::logger> Logger::s_clientLogger = nullptr;
	std::shared_ptr<GUILogger> Logger::s_guiLogger = nullptr;
	bool Logger::s_GUILoggerInitialized = false;

	void Logger::Initialize()
	{
		s_coreLogger = spdlog::stdout_color_mt("HELIOS ENGINE");
		s_coreLogger->set_level(spdlog::level::trace);
		s_coreLogger->set_pattern("[%T](%n) %^%v%$");

		s_clientLogger = spdlog::stdout_color_mt("GAME");
		
		s_clientLogger->set_level(spdlog::level::trace);
		s_clientLogger->set_pattern("%^[%T] %n: %v%$");
	}

	void Logger::SetupGUILogger(std::vector<std::pair<GUILogger::ELevelType, std::string>>& a_logger)
	{
		if(s_GUILoggerInitialized)
		{
			HE_CORE_WARN("GUI logger already initialized!");
			return;
		}
		
		s_guiLogger = std::make_shared<GUILogger>(a_logger);
		s_coreLogger->sinks().push_back(std::make_shared<GUISink>(s_guiLogger));
		s_clientLogger->sinks().push_back(std::make_shared<GUISink>(s_guiLogger));

		s_GUILoggerInitialized = true;
	}

	void Logger::RemoveGUILogger()
	{
		s_guiLogger = std::shared_ptr<GUILogger>();
		s_guiLogger = nullptr;
	}

	GUISink::GUISink(const std::shared_ptr<GUILogger>& a_guiLogger)
		: m_guiLogger(a_guiLogger)
	{
		
	}

	void GUISink::sink_it_(const spdlog::details::log_msg& msg)
	{
		if (m_guiLogger != nullptr)
		{
			const int warningLevel = static_cast<int>(msg.level);
			const std::chrono::system_clock::time_point timePoint = msg.time;
			const std::time_t t = std::chrono::system_clock::to_time_t(timePoint);
			const std::tm now_tm = *std::localtime(&t);
			const std::string time = "[" + std::to_string(now_tm.tm_hour) + ":" + std::to_string(now_tm.tm_min) + ":" + std::to_string(now_tm.tm_sec) + "]";
			
			spdlog::memory_buf_t formatted;
			formatter_->format(msg, formatted);
			const std::string correctFormat = time + "| " + fmt::to_string(msg.logger_name) + " | " + fmt::to_string(msg.payload);
			m_guiLogger->LogToWindow(static_cast<GUILogger::ELevelType>(warningLevel), correctFormat);
		}
	}

	void GUISink::flush_()
	{
		std::cout << std::flush;
	}
}