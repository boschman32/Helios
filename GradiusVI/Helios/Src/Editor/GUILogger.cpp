#include  "hepch.h"
#include "Editor/GUILogger.h"

namespace Helios
{
	GUILogger::GUILogger(std::vector<std::pair<ELevelType, std::string>>& a_editorLogger)
		: m_editorLogger(a_editorLogger)
	{
	}

	void GUILogger::LogToWindow(ELevelType a_level, std::string a_msg) const
	{
		m_editorLogger.insert(m_editorLogger.begin(), std::pair<ELevelType, std::string>(a_level, a_msg));
	}
}

