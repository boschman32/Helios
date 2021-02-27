#pragma once
#include "Editor/EditorWindow.h"
namespace Helios
{
    struct FileData;
    class Scene;
	class ComponentEditor;
	class Entity;
	class ContentBrowser;
	
	class SceneGraph : public EditorWindow
	{
	public:
		SceneGraph(const std::string& a_name, ContentBrowser* a_contentBrowser, float a_xPos, float a_yPos, float a_xScale, float a_yScale);

	protected:
		void ShowContext() override;
	private:
		ContentBrowser* m_contentBrowser;
		std::map<Entity*, Entity*> m_parentObjectPairs;
        bool m_deletingPrefab;
        const FileData* m_prefabFileToDelete;

		void LoadPrefab();
        void DeletePrefab(const FileData& a_prefabPath);
		void HandleDragAndDropRoot();
		void DrawEntityNode(Entity* a_entity);
		void HandleDragAndDrop(Entity& a_gameObject, const std::string& a_tooltipText);
	};
}

