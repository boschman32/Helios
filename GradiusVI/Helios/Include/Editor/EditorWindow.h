#pragma once
#include "Core/Core.h"

namespace Helios
{
	class EditorWindow
	{
	public:
		EditorWindow(const std::string& a_windowName, float a_xPos, float a_yPos, float a_xSize, float a_ySize);
        virtual ~EditorWindow() = default;

		void SetWindowFlags(ImGuiWindowFlags a_newFlags);
		void DisplayWindow();
	protected:
		virtual void ShowContext() = 0;
	private:
		std::string m_windowName;
		Vec2 m_size;
		Vec2 m_position;
		ImGuiWindowFlags m_flags = 0;
	};
}
