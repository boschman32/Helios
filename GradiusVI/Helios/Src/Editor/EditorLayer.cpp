#include "hepch.h"

#include "Editor/EditorLayer.h"

#include "Core/EntityComponent/EntityManager.h"

#include "SceneManagement/SceneManager.h"
#include "SceneManagement/EditorSceneLoader.h"

#include "Editor/ContentBrowser.h"
#include "Editor/MenuBar.h"
#include "Editor/ComponentEditor.h"
#include "Editor/SceneGraph.h"

namespace Helios
{
    EditorLayer::EditorLayer()
    {
        m_contentBrowser = std::make_unique<ContentBrowser>("Content Browser", -1.f, 0.6f, 1.0f, 0.205f);
        m_contentBrowser->SetWindowFlags(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        m_componentEditor = std::make_unique<ComponentEditor>("Inspector", 0.7f, -1.0f, 0.15f, 0.8f, *m_contentBrowser.get());
        m_componentEditor->SetWindowFlags(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        m_menuBar = std::make_unique<MenuBar>("MenuBar", -0.71f, -1.0f, 0.71f, 0.1f);
        m_menuBar->SetWindowFlags(ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize);

        m_sceneGraph = std::make_unique<SceneGraph>("Hierarchy", m_contentBrowser.get(), -1.f, -1.f, 0.15f, 0.8f);
        m_sceneGraph->SetWindowFlags(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    }

    void EditorLayer::OnAttach()
    {
        SetStyle();

        Logger::SetupGUILogger(m_contentBrowser->GetLog());
        HE_CORE_INFO("Editor windows attached!");
    }

    void EditorLayer::OnDetach()
    {
        Logger::RemoveGUILogger();
    }

    void EditorLayer::OnUpdate(float)
    {

    }

    void EditorLayer::OnImGUIRender()
    {
        m_menuBar->DisplayWindow();
        m_sceneGraph->DisplayWindow();
        m_contentBrowser->DisplayWindow();
        m_componentEditor->DisplayWindow();
    }

    void EditorLayer::OnEvent()
    {
        /*if (a_evt.type == sf::Event::KeyPressed)
        {
            switch (a_evt.key.code)
            {
            case sf::Keyboard::B:
                m_contentBrowser->DidFolderChange(m_contentBrowser->GetCurrentFolder().string());
                break;
            case sf::Keyboard::Delete:
            {
                Scene& activeScene = SceneManager::GetInstance().GetActiveScene();
                Entity* selectedEntity = activeScene.GetSelectedEntity();

                if (selectedEntity != nullptr)
                {
                    EntityManager::GetInstance().DestroyEntityDirectly(selectedEntity, activeScene);
                }
            }
            break;
            case sf::Keyboard::S:
            {
                if (a_evt.key.control)
                {
                    Scene& activeScene = SceneManager::GetInstance().GetActiveScene();
                    if (activeScene.IsValid())
                    {
                        EditorSceneLoader::SaveScene(activeScene);
                    }
                }
            }
            break;
            case sf::Keyboard::Z:
                if (a_evt.key.control)
                {
                    EditorHistory::UndoChange();
                }
                break;
            case sf::Keyboard::Y:
                if (a_evt.key.control)
                {
                    EditorHistory::RedoChange();
                }
                break;
            }
        }*/
    }

    void EditorLayer::SetStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 1.0f;

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.67f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.27f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.27f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.45f, 0.46f, 0.48f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.45f, 0.46f, 0.47f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    }
}
