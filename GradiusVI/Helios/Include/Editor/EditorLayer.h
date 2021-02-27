#pragma once
#include "Core/Layer.h"

namespace Helios
{
    class ContentBrowser;
    class ComponentEditor;
    class MenuBar;
    class SceneGraph;

    class EditorLayer : public Layer
    {
    public:
        EditorLayer();

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float a_deltaTime) override;
        void OnEvent() override;
        void OnImGUIRender() override;
    private:
        std::unique_ptr<ContentBrowser> m_contentBrowser;
        std::unique_ptr<ComponentEditor> m_componentEditor;
        std::unique_ptr<MenuBar> m_menuBar;
        std::unique_ptr<SceneGraph> m_sceneGraph;

        void SetStyle();
    };
}
