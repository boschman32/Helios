#include "hepch.h"
#include "Core/Prefab.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/EntityManager.h"
#include "SceneManagement/SceneManager.h"
#include "Editor/SceneGraph.h"
#include "Editor/ComponentEditor.h"
#include "Editor/ContentBrowser.h"
#include "Utils/Serializer.h"
#include "Components/Transform.h"
#include "imgui_internal.h"

namespace Helios
{
	SceneGraph::SceneGraph(const std::string& a_name, ContentBrowser* a_contentBrowser, float a_xPos, float a_yPos, float a_xScale, float a_yScale)
		: EditorWindow(a_name, a_xPos, a_yPos, a_xScale, a_yScale), m_contentBrowser(a_contentBrowser), m_deletingPrefab(false)
	{
	}

	void SceneGraph::HandleDragAndDropRoot()
	{
		if (ImGui::BeginDragDropTargetCustom(ImGui::GetWindowAllowedExtentRect(ImGui::GetCurrentWindow()), ImGui::GetID("hierarchy_drag_drop")))
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				const Entity* otherObjectPtr = static_cast<Entity*>(payload->Data);
				std::vector<Entity*>& gameObjects = SceneManager::GetInstance().GetActiveScene().GetEntitiesInScene();
				for (auto g : gameObjects)
				{
					if (g->GetID() == otherObjectPtr->GetID())
					{
						EntityID id = otherObjectPtr->GetID();
						const auto og = EntityManager::GetInstance().GetEntityByUniqueId(id);
						m_parentObjectPairs[og] = nullptr;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	void SceneGraph::DrawEntityNode(Entity* a_entity)
	{
		const bool selected = SceneManager::GetInstance().GetActiveScene().GetSelectedEntity() == a_entity;
		const std::string name = a_entity->GetName();

		const ImGuiTreeNodeFlags flags = (selected ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None) |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (a_entity->GetTransform().GetChildrenCount() > 0 ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Bullet);
		const std::string idc = std::to_string(a_entity->GetID().Value());
		bool open;
		if (a_entity->HasPrefab())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(104.f / 255.f, 151.f / 255.f, 227.f / 255.f, 1.f));
			open = ImGui::TreeNodeEx(idc.c_str(), flags, "%s", name.c_str());
			ImGui::PopStyleColor();
		}
		else
		{
			open = ImGui::TreeNodeEx(idc.c_str(), flags, "%s", name.c_str());
		}

		if (ImGui::IsItemClicked())
		{
			SceneManager::GetInstance().GetActiveScene().SetSelectedEntity(a_entity);
		}
		HandleDragAndDrop(*a_entity, name);

		if (open)
		{
			for (const auto child : a_entity->GetTransform().GetChildren())
			{
				DrawEntityNode(&child->GetOwner());
			}
			ImGui::TreePop();
		}
	}

	void SceneGraph::HandleDragAndDrop(Entity& a_gameObject, const std::string& a_tooltipText)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				const auto otherObjectPtr = static_cast<Entity*>(payload->Data);
				std::vector<Entity*>& gameObjects = SceneManager::GetInstance().GetActiveScene().GetEntitiesInScene();
				for (auto g : gameObjects)
				{
					if (g->GetID() == otherObjectPtr->GetID())
					{
						const EntityID& id = otherObjectPtr->GetID();
						const auto og = EntityManager::GetInstance().GetEntityByUniqueId(id);
						m_parentObjectPairs[og] = &a_gameObject;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Entity", &a_gameObject, sizeof(Entity));
			ImGui::Text("%s", a_tooltipText.c_str());
			ImGui::EndDragDropSource();
		}
	}

	void SceneGraph::LoadPrefab()
	{
		std::vector<FileData> files = { FileData() };
		const std::vector<FileData> filesWithExtension = m_contentBrowser->GetAllFilesOfExtensions({ ".prefab" });
		files.insert(files.end(), filesWithExtension.begin(), filesWithExtension.end());

		static FileData curFile = files[0];
		if (ImGui::BeginCombo("Prefabs", curFile.m_fileName.c_str()))
		{
			for (auto& f : files)
			{
				const bool isSelected = (curFile.m_fileName == f.m_fileName);
				if (ImGui::Selectable(f.m_fileName.c_str(), isSelected))
				{
					curFile = f;
				}
				if (isSelected)
				{
					// Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
					ImGui::SetItemDefaultFocus();
				}

			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Load Prefab"))
		{
			if (curFile.m_fileName != file_data::NONE_FILE)
			{
				const std::string prefabFilePath = curFile.m_filePath;

				Entity* newPrefab = PrefabUtility::LoadPrefab(prefabFilePath)->Spawn(Vec3{ 0.f }, true);
				if (newPrefab != nullptr)
				{
					const std::string newValue = Serializer::SerializePrefab(*newPrefab);
					EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_ENTITY, "", newValue);
				}
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.f));
		if (ImGui::Button("Delete Prefab"))
		{
			if (curFile.m_fileName != file_data::NONE_FILE)
			{
				m_deletingPrefab = true;
				m_prefabFileToDelete = &curFile;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::PopStyleColor();

		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
	}

	void SceneGraph::DeletePrefab(const FileData& a_prefabPath)
	{
		ImGui::Text("Are you sure you want to delete this prefab (%s)?", a_prefabPath.m_fileName.c_str());
		if (ImGui::Button("Yes"))
		{
			if (a_prefabPath.m_fileName != file_data::NONE_FILE)
			{
				const std::string prefabFilePath = a_prefabPath.m_filePath;

				if (std::filesystem::exists(prefabFilePath))
				{
					std::filesystem::remove(prefabFilePath);
					m_contentBrowser->DidFolderChange(ExtractFolderFromFilePath(prefabFilePath));
					m_prefabFileToDelete = nullptr;
				}
			}
			m_deletingPrefab = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			m_deletingPrefab = false;
			ImGui::CloseCurrentPopup();
		}
	}

	void SceneGraph::ShowContext()
	{
		Scene& activeScene = SceneManager::GetInstance().GetActiveScene();
		HandleDragAndDropRoot();

		ImGui::BeginChild("Entities", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMax().y * 0.87f));
		{
			for (auto e : activeScene.GetEntitiesInScene())
			{
				if (e->GetTransform().GetParent() == nullptr)
				{
					DrawEntityNode(e);
				}
			}
		}
		ImGui::EndChild();

		ImGui::BeginChild("CreateEntity");
		{
			if (ImGui::Button("Create New Entity", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMin().y)))
			{
				Entity* newEntity = EntityManager::GetInstance().CreateEntityDirectly(activeScene);
				const std::string newValue = Serializer::Serialize(*newEntity, true);
				EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_ENTITY, "", newValue);
			}
			bool showModal = false;
			if (ImGui::Button("Load Prefab", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMin().y)))
			{
				showModal = true;
			}

			if (showModal)
			{
				ImGui::OpenPopup("Prefab Creator");
			}
			if (ImGui::BeginPopupModal("Prefab Creator", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				LoadPrefab();
				ImGui::EndPopup();
			}

			if (m_deletingPrefab)
			{
				ImGui::OpenPopup("Delete?");
			}
			if (ImGui::BeginPopupModal("Delete?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				DeletePrefab(*m_prefabFileToDelete);
				ImGui::EndPopup();
			}
		}
		ImGui::EndChild();

		for (const auto& pair : m_parentObjectPairs)
		{
			auto secondParent = pair.second;
			if (pair.second != nullptr)
			{
				secondParent = &pair.second->GetTransform().GetOwner();
			}
			if (pair.first != nullptr && secondParent != nullptr)
			{
				if (secondParent == pair.first)
				{
					// When trying to parent a parent into one of the children, swap their positions in the hierarchy
					secondParent->GetTransform().SetParent(&pair.first->GetTransform());
				}
				else
				{
					//pair.second->AddChild(pair.first);
					pair.first->GetTransform().SetParent(&secondParent->GetTransform());
				}
			}
			else
			{
				pair.first->GetTransform().ResetParent();
			}
		}
		m_parentObjectPairs.clear();

	}
}
