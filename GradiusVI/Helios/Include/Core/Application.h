#pragma once
#include "LayerStack.h"
#include "EntityComponent/EntityManager.h"
class Window;

namespace Helios
{
    class EditorLayer;

    class Application
    {
    public:
        Application();
        virtual ~Application() = default;

        void PushLayer(std::unique_ptr<Layer> a_layer);
        void PushOverlay(std::unique_ptr<Layer> a_overlay);
        void PopLayer(Layer& a_layer);
        void PopOverlay(Layer& a_overlay);
#ifdef WITH_EDITOR
        void ShowEditor(bool a_show);
#endif

        bool Run(HINSTANCE a_hInstance);

        void Play(bool a_profile = false);
        void Stop();
        bool IsInEditMode() const { return m_showingEditor; }
        bool IsPlaying() const { return m_isPlaying; };
        bool IsPaused() const { return m_isPaused; }

        Window& GetWindow() const { return *m_window; }

        static Application& Get() { return *ms_instance; }
    private:
        static Application* ms_instance;

        LayerStack m_layerStack;
        std::shared_ptr<Window> m_window;

        EditorLayer* m_editorLayer = nullptr;
        std::string m_storedSceneJSON;

        bool m_showingEditor;
        bool m_isPlaying;
        bool m_isPaused;
        bool m_isProfiling;

        bool DX12Run(HINSTANCE a_hInstance);

        void UpdateViewport();
        void GetViewportRanges(float& a_x, float& a_y, float& a_width, float& a_height) const;
        void CheckForEvents();
    };

    //To be defined by client.
    Application* CreateApplication();
}
