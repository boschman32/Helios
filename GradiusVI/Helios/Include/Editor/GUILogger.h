#pragma once

namespace Helios
{
	class GUILogger
	{
	public:
		enum class ELevelType
		{
			LevelType_Trace,
			LevelType_Debug,
			LevelType_Info,
			LevelType_Warn,
			LevelType_Error,
			LevelType_Critical,
			LevelType_Off
		};

		GUILogger(std::vector<std::pair<ELevelType, std::string>>& a_editorLogger);
		void LogToWindow(ELevelType a_level, std::string a_msg) const;
	private:
		std::vector<std::pair<ELevelType, std::string>>& m_editorLogger;
	};
}