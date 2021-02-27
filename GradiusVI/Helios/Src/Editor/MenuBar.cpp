#include "hepch.h"
#include "Editor/MenuBar.h"
#include "Editor/pfd.h"
#include "SceneManagement/SceneManager.h"
#include "SceneManagement/EditorSceneLoader.h"
#include "Core/Application.h"
#include "Core/ResourceManager.h"
#include "Editor/EditorHistory.h"
#include "Core/CollisionManager.h"
#include <../FontAwesome/FontAwesome.h>
#include "Core/TFD.h"

namespace Helios
{
	MenuBar::MenuBar(const std::string& a_windowName, float a_xPos, float a_yPos, float a_xSize, float a_ySize)
		: EditorWindow(a_windowName, a_xPos, a_yPos, a_xSize, a_ySize)
	{
		
	}

	void MenuBar::ShowContext()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				const std::vector<std::string> filters
				{
					"Scene", "*" + std::string(SCENE_EXTENSION_NAME)
				};

				if (ImGui::MenuItem("New Scene..."))
				{
					CollisionManager::GetInstance().Reset();
					EditorHistory::ClearHistory();
					SceneManager::GetInstance().CreateScene();
					EntityManager::GetInstance().DestroyAll();
					ResourceManager::GetInstance().ReleaseAllResources();
				}

				if (ImGui::MenuItem("Save Scene As..."))
				{
					char const* lFilterPatterns[2] = { "*.scene", "*.text" };

					const char * lTheOpenFileName = tinyfd_saveFileDialog(
						"Save Scene As...",
						"./Assets/",
						1,
						lFilterPatterns,
						NULL);

					if(lTheOpenFileName != nullptr)
					{
						const std::string filepath = std::string(lTheOpenFileName);
						if (!filepath.empty())
						{
							std::string relative = std::filesystem::relative(filepath).string();

							const std::size_t hasExtension = relative.find_last_not_of(".scene");
							if (hasExtension != std::string::npos)
							{
								relative = relative.substr(0, hasExtension + 1);
							}
							relative += SCENE_EXTENSION_NAME;

							EditorSceneLoader::SaveSceneAs(SceneManager::GetInstance().GetActiveScene(), relative);
						}
						else
						{
							HE_CORE_CRITICAL("HOLD UP, FILE PATH IS EMPTY!");
						}
					}
				}

				if (ImGui::MenuItem("Save Scene...", "CTRL+S", false, SceneManager::GetInstance().GetActiveScene().IsValid()))
				{
					EditorSceneLoader::SaveScene(SceneManager::GetInstance().GetActiveScene());
				}

				if (ImGui::MenuItem("Open Scene..."))
				{
					CollisionManager::GetInstance().Reset();
					EditorHistory::ClearHistory();
					EntityManager::GetInstance().DestroyAll();
					ResourceManager::GetInstance().ReleaseAllResources();

					char const* lFilterPatterns[2] = { "*.scene", "*.text" };

					const char* lTheOpenFileName = tinyfd_openFileDialog(
						"Save Scene As...",
						"./Assets/",
						1,
						lFilterPatterns,
						NULL,
						0);

					if(lTheOpenFileName != nullptr)
					{
						const std::string filepath = std::string(lTheOpenFileName);
						if (!filepath.empty())
						{
							EditorSceneLoader::OpenScene(filepath);
						}
					}
				}
				ImGui::EndMenu();
			}
			if (Application::Get().IsPaused())
			{
				ImGui::SameLine(ImGui::GetWindowContentRegionWidth() / 2.f);
				if (ImGui::Button(ICON_FA_STOP))
				{
					Application::Get().Stop();
				}
			}
			else if (!Application::Get().IsPlaying())
			{
				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Undo", "Ctrl + Z"))
					{
						EditorHistory::UndoChange();
					}
					if (ImGui::MenuItem("Redo", "Ctrl + Y"))
					{
						EditorHistory::RedoChange();
					}
					Scene& activeScene = SceneManager::GetInstance().GetActiveScene();
					Entity* selectedEntity = activeScene.GetSelectedEntity();
					bool enabled = selectedEntity != nullptr;
					if (ImGui::MenuItem("Delete Entity", "", false, enabled))
					{
						if (selectedEntity != nullptr)
						{
							EntityManager::GetInstance().DestroyEntityDirectly(selectedEntity, activeScene);
						}
					}
					ImGui::EndMenu();
				}
				ImGui::SameLine(ImGui::GetWindowContentRegionWidth() / 2.f);
				if (ImGui::Button(ICON_FA_PLAY))
				{
					Application::Get().Play();
				}
				if (ImGui::Button(ICON_FA_BUG))
				{
					Application::Get().Play(true);
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					{
						ImGui::SetTooltip("Play the game and start a capture for Optick Profiling. \nStopping the game will save the capture and save it in the project folder.");
					}
					ImGui::EndTooltip();
				}
				
			}

			ImGui::EndMenuBar();
		}
	}
}
