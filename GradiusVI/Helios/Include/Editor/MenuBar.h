#include "EditorWindow.h"
namespace Helios
{
	class MenuBar : public EditorWindow
	{
	public:
		MenuBar(const std::string& a_windowName, float a_xPos, float  a_yPos, float  a_xSize, float a_ySize);
		void ShowContext() override;
		
	};
}
