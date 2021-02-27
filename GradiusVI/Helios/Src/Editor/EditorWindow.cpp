#include "hepch.h"
#include "Editor/EditorWindow.h"

namespace Helios
{
	EditorWindow::EditorWindow(const std::string& a_windowName, float xPos, float yPos, float xSize, float ySize)
        : m_windowName(std::move(a_windowName)), m_size(xSize, ySize), m_position(xPos, yPos)
	{

	}

	void EditorWindow::SetWindowFlags(ImGuiWindowFlags a_newFlags)
	{
		m_flags = a_newFlags;
	}

	void EditorWindow::DisplayWindow()
	{
		ImGui::SetNextWindowSizeRelative(ImVec2(m_size.x, m_size.y));
		ImGui::SetNextWindowPosNDC(ImVec2(m_position.x, m_position.y));
		ImGui::Begin(m_windowName.c_str(),nullptr, m_flags);
		ShowContext();
		ImGui::End();
	}
}
