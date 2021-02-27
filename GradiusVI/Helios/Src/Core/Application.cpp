#include "hepch.h"
#include "Core/Application.h"

#include "Utils/Serializer.h"
#include "Utils/Timer.h"

#ifdef WITH_EDITOR
//Editor
#include "Editor/EditorLayer.h"
#endif

//RTTR needs to register components otherwise we can't serialize/deserialize them.
//And for some classes we want to know beforehand what properties it has (before it has been instantiated). 
#include "Core/CoreRegistration.h"

//Collision
#include "Core/CollisionManager.h"

//AudioTesting
#include "Core/AudioEngine.h"

//Scenes
#include "SceneManagement/SceneManager.h"

//DirectX_12 - Rendering
#include "Renderer/DX12Renderer.h"
#include "Renderer/Window.h"

#include "Core/InputManager.h"
#include "Editor/EditorHistory.h"
#include "SceneManagement/EditorSceneLoader.h"

namespace Helios
{
    Application* Application::ms_instance = nullptr;

    Application::Application()
        : m_showingEditor(false), m_isPlaying(false), m_isPaused(false), m_isProfiling(false)
    {
        HE_CORE_ASSERT(!ms_instance, "Application already exists!");
        ms_instance = this;

        UpdateViewport();

        AudioEngine::CAudioEngine::Init();
    }

    void Application::PushLayer(std::unique_ptr<Layer> a_layer)
    {
        m_layerStack.PushLayer(std::move(a_layer));
    }

    void Application::PushOverlay(std::unique_ptr<Layer> a_overlay)
    {
        m_layerStack.PushOverlay(std::move(a_overlay));
    }

    void Application::PopLayer(Layer& a_layer)
    {
        m_layerStack.PopLayer(a_layer);
    }

    void Application::PopOverlay(Layer& a_overlay)
    {
        m_layerStack.PopLayer(a_overlay);
    }

#ifdef WITH_EDITOR
    void Application::ShowEditor(bool a_show)
    {
        m_showingEditor = a_show;
        if (m_editorLayer != nullptr)
        {
            if (!m_showingEditor)
            {
                m_editorLayer->SetEnabled(false);
            }
            else
            {
                m_editorLayer->SetEnabled(true);
            }
        }

        UpdateViewport();
    }
#endif

    bool Application::Run(HINSTANCE a_hInstance)
    {
        // Create the controller
        std::unique_ptr<Controller> controller = std::make_unique<Controller>();

        // Set the input keys.

        Button Pause;
        Pause.AddKey(Key::Escape);
        Pause.SetName("Pause");
        controller->AddButton(Pause);

        Button Undo;
        Undo.AddKey(Key::Z);
        Undo.SetName("Undo");
        controller->AddButton(Undo);

        Button Redo;
        Redo.AddKey(Key::Y);
        Redo.SetName("Redo");
        controller->AddButton(Redo);

        Button Delete;
        Delete.AddKey(Key::Delete);
        Delete.SetName("Delete");
        controller->AddButton(Delete);

        Button save;
        save.AddKey(Key::S);
        save.SetName("Save");
        controller->AddButton(save);

        Button Control;
        Control.AddKey(Key::ControlKey);
        Control.SetName("Control");
        controller->AddButton(Control);

        InputManager::AddController(std::move(controller));

        return DX12Run(a_hInstance);
    }

    void Application::Play(bool a_profile)
    {
#ifdef WITH_EDITOR
        ShowEditor(false);
#endif
        m_isPlaying = true;
        m_isPaused = false;
        AudioEngine::CAudioEngine::GetInstance().SetPausedForAllChannels(false);

        if (a_profile)
        {
            OPTICK_START_CAPTURE();
            m_isProfiling = true;
        }

        EntityManager::GetInstance().Start();

        //Tell any layer that we are playing the game.
        for (const auto& l : m_layerStack)
        {
            l->OnPlay();
        }

#ifdef WITH_EDITOR
        Scene& active = SceneManager::GetInstance().GetActiveScene();
        m_storedSceneJSON = Serializer::Serialize(active);
#endif
    }

    void Application::Stop()
    {
#ifdef WITH_EDITOR
        if (!m_showingEditor)
        {
            ShowEditor(true);
        }
#endif

        if (m_isProfiling)
        {
            OPTICK_STOP_CAPTURE();

            const std::string optickSavePath = "../OptickCaptures/";
            if (!std::filesystem::exists(optickSavePath))
            {
                std::filesystem::create_directory(optickSavePath);
            }
            OPTICK_SAVE_CAPTURE((optickSavePath + "GradiusVI").c_str());
        }

        m_isPlaying = false;
        m_isPaused = false;
        m_isProfiling = false;

        AudioEngine::CAudioEngine::GetInstance().StopAllChannels();

        //Tell any layer that we are stopping the game.
        for (const auto& l : m_layerStack)
        {
            l->OnStop();
        }

        CollisionManager::GetInstance().Reset();
        EntityManager::GetInstance().DestroyAll();

#ifdef WITH_EDITOR
        Scene& scene = SceneManager::GetInstance().CreateScene();
        Deserializer::DeserializeScene(scene, m_storedSceneJSON);
#endif
    }

    bool Application::DX12Run(HINSTANCE a_hInstance)
    {
        DX12Renderer::Create(a_hInstance, L"testy", 1280, 720);
        DX12Renderer& renderer = DX12Renderer::GetInstance();

        Timer m_deltaTimer;
        int frameCount = 0;
        float totalTime = 0;

        m_window = renderer.GetWindowByName(L"testy");
#ifdef WITH_EDITOR
        m_editorLayer = dynamic_cast<EditorLayer*>(m_layerStack.PushOverlay(std::make_unique<EditorLayer>()));
        ShowEditor(true);
#else
        Play();
#endif

        MSG msg = {};
        while (msg.message != WM_QUIT)
        {
            //Profiling
            OPTICK_FRAME("MainThread");

            //--- Input polling ---
            CheckForEvents();

            if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }

            if (m_window != nullptr)
            {
                renderer.NewFrame(m_window);
            }

            //--- Updating ---
            OPTICK_EVENT();

            const float deltaTime = m_deltaTimer.GetElapsedTime();
            totalTime += deltaTime;

            CollisionManager::GetInstance().Render();

            if (m_isPlaying)
            {
                EntityManager::GetInstance().Update(deltaTime);

                //Update the layers
                for (const auto& layer : m_layerStack)
                {
                    if (layer->IsEnabled())
                    {
                        layer->OnUpdate(deltaTime);
                    }
                }
            }

            if (m_isPlaying)
            {
                EntityManager::GetInstance().Update(deltaTime);

                //Update the layers
                for (const auto& layer : m_layerStack)
                {
                    if (layer->IsEnabled())
                    {
                        layer->OnUpdate(deltaTime);
                    }
                }
            }

            if (m_isPlaying)
            {
                //Render first! Then do collision checks
                CollisionManager::GetInstance().CollisionChecks();
            }

            for (const auto& layer : m_layerStack)
            {
                if (layer->IsEnabled())
                {
                    layer->OnImGUIRender();
                }
            }

            frameCount++;
            if (totalTime >= 1.0f)
            {
                m_window->SetDisplayName("Helios Editor | Scene: " + SceneManager::GetInstance().GetActiveScene().GetSceneName() + " | FPS: " + std::to_string(float(frameCount) / totalTime));
                totalTime = 0;
                frameCount = 0;
            }

            if (m_window != nullptr)
            {
                std::vector<Entity*> pEntities = EntityManager::GetInstance().GetAllEntities();
                float x, y, width, height;
                GetViewportRanges(x, y, width, height);
                DirectX::XMFLOAT4 viewportRanges(x, y, width, height);
                renderer.RenderToWindow(m_window, viewportRanges, pEntities);
                /*renderer.RenderDX12LibScene(pWindow);*/
            }
        }
        DX12Renderer::Destroy();

        OPTICK_SHUTDOWN();
        AudioEngine::CAudioEngine::Shutdown();
        EntityManager::GetInstance().DestroyAll();
        PrefabUtility::UnloadAll();

        return true;
    }

    void Application::UpdateViewport()
    {
        float x, y, width, height;
        GetViewportRanges(x, y, width, height);
    }

    void Application::GetViewportRanges(float& a_x, float& a_y, float& a_width, float& a_height) const
    {
        if (m_showingEditor)
        {
            a_x = 0.15f;
            a_y = 0.02f;
            a_width = 0.85f;
            a_height = 0.8f;
        }
        else
        {
            a_x = 0.f;
            a_y = 0.f;
            a_width = 1.f;
            a_height = 1.f;
        }
    }

    void Application::CheckForEvents()
    {
#ifdef WITH_EDITOR
        Controller* mainInput = InputManager::GetController(0);
        if (mainInput != nullptr)
        {
            if (mainInput->GetButtonOnce("Pause"))
            {
                if (!m_showingEditor && m_isPlaying)
                {
                    m_isPlaying = false;
                    m_isPaused = true;
                    AudioEngine::CAudioEngine::GetInstance().SetPausedForAllChannels(true);
                }
                else if (m_showingEditor && m_isPaused)
                {
                    m_isPlaying = true;
                    AudioEngine::CAudioEngine::GetInstance().SetPausedForAllChannels(false);
                    m_isPaused = false;
                }

                ShowEditor(!m_showingEditor);
            }

            if (mainInput->GetButtonOnce("Delete"))
            {
                Scene& activeScene = SceneManager::GetInstance().GetActiveScene();
                Entity* selectedEntity = activeScene.GetSelectedEntity();

                if (selectedEntity != nullptr)
                {
                    EntityManager::GetInstance().DestroyEntityDirectly(selectedEntity, activeScene);
                }
            }

            if (mainInput->GetButtonDown("Control"))
            {
                if (mainInput->GetButtonOnce("Undo"))
                {
                    EditorHistory::UndoChange();
                }
                else if (mainInput->GetButtonOnce("Redo"))
                {
                    EditorHistory::RedoChange();
                }
                else if (mainInput->GetButtonOnce("Save"))
                {
                    Scene& activeScene = SceneManager::GetInstance().GetActiveScene();
                    if (activeScene.IsValid())
                    {
                        EditorSceneLoader::SaveScene(activeScene);
                    }
                }
            }
        }
#endif
    }
}
