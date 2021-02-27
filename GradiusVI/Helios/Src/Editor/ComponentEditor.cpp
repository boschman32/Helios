#include "hepch.h"
#include "Editor/ComponentEditor.h"

#include "Core/EntityComponent/EntityManager.h"
#include "Core/EntityComponent/Component.h"
#include "Core/Prefab.h"

#include "SceneManagement/SceneManager.h"

#include "Components/AudioSource.h"
#include "Components/Transform.h"

#include "Utils/FileUtils.h"

#include <../FontAwesome/FontAwesome.h>
#include "Core/Core.h"
#include "Core/Application.h"
#include "Components/LuaComponent.h"
#include "Components/MeshRenderer.h"

#include "Core/CoreSerializationNames.h"

namespace Helios
{
	ComponentEditor::ComponentEditor(const std::string& a_windowName, float a_xPos, float a_yPos, float a_xSize, float a_ySize, ContentBrowser& a_contentBrowser)
		: EditorWindow(a_windowName, a_xPos, a_yPos, a_xSize, a_ySize),
		m_contentBrowser(&a_contentBrowser)
	{
	}

	void ComponentEditor::ShowContext()
	{
		Entity* selectedEntity = SceneManager::GetInstance().GetActiveScene().GetSelectedEntity();
		if (selectedEntity != nullptr)
		{
			int index = 0;
			//Create name property.
			ImGui::BeginChild("EntityName", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMax().y * 0.045f));

			bool entityEnabled = selectedEntity->IsEnabled();
			if (ImGui::Checkbox("", &entityEnabled))
			{
				selectedEntity->SetEnabled(entityEnabled);
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
			rttr::property nameProperty = rttr::type::get(*selectedEntity).get_property(S_ENTITY_PROP_NAME);
			GenerateUIForString("", selectedEntity->GetName().c_str(), selectedEntity, nameProperty, index);
			ImGui::PopItemWidth();
			ImGui::EndChild();

			//Create component properties.
			ImGui::Separator();
			ImGui::BeginChild("ComponentScroller", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMax().y * 0.70f));

			for (auto c : selectedEntity->GetAllComponents())
			{
				index++;
				const rttr::type t = rttr::type::get(*c);
				const std::string name = t.get_name().to_string();

				if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
				{
					ImGui::PushID(index * 3000);
					if (name != "Transform")
					{
						static const int ButtonWidth = 20;
						ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - ButtonWidth);
						if (ImGui::SmallButton(ICON_FA_TRASH))
						{
							const std::string oldValue = Serializer::Serialize(*c, true);
							EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, "");
							selectedEntity->RemoveComponent(c);
							ImGui::PopID();
							continue;
						}

						ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - (ButtonWidth * 2));
						bool enabled = c->IsEnabled();
						if (ImGui::Checkbox("", &enabled))
						{
							c->SetEnabled(enabled);
						}
					}

					DrawUIObject(t, *c, index);
					ImGui::PopID();
				}
			}
			ImGui::EndChild();

			ImGui::NewLine();
			ImGui::BeginChild("Extra options");
			if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowContentRegionWidth(), 20)))
			{
				ImGui::OpenPopup("Components");
			}

			if (ImGui::BeginPopup("Components"))
			{
				rttr::array_range<rttr::type> componentTypes = rttr::type::get<Component>().get_derived_classes();
				if (ImGui::BeginMenu("Core"))
				{
					for (auto& c : componentTypes)
					{
						bool cont = true;
						if(c.get_name() == "Transform" || c.get_name() == "Scriptable Component")
						{
							cont = false;
						}
						for (auto& d : c.get_base_classes())
						{
							if (d.get_name() == "Scriptable Component")
							{
								cont = false;
							}

						}
						if (cont)
						{
							if (ImGui::MenuItem(c.get_name().to_string().c_str()))
							{
								rttr::variant obj = c.create({ *selectedEntity,
									ComponentID { selectedEntity->GetComponentIDCount() } });
								Component* comp = selectedEntity->AddComponent(std::unique_ptr<Component>(obj.get_value<Component*>()));

								const std::string newValue = Serializer::Serialize(*comp, true);
								EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, "", newValue);
							}
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Gameplay"))
				{
					for (auto& c : componentTypes)
					{

						if (c.get_name().to_string() == "Scriptable Component")
						{
							for (auto& d : c.get_derived_classes())
							{
								if (d.get_name() != "Transform")
								{
									if (ImGui::MenuItem(d.get_name().to_string().c_str()))
									{
										rttr::variant obj = d.create({ *selectedEntity,
											ComponentID { selectedEntity->GetComponentIDCount() } });
										Component* comp = selectedEntity->AddComponent(std::unique_ptr<Component>(obj.get_value<Component*>()));

										const std::string newValue = Serializer::Serialize(*comp, true);
										EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, "", newValue);
									}

								}
							}
						}
					}
					ImGui::EndMenu();

				}

				ImGui::EndPopup();
			}
			if (Application::Get().IsInEditMode() && !Application::Get().IsPaused())
			{
				Entity* rootEntity = &selectedEntity->GetTransform().GetRoot()->GetOwner();
				if (rootEntity != nullptr)
				{
					if (!rootEntity->HasPrefab())
					{
						ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
						if (ImGui::Button("Create Prefab", ImVec2(ImGui::GetWindowContentRegionWidth(), 20)))
						{
							const std::string prefabPath = DEFAULT_PREFAB_PATH + rootEntity->GetName() + PREFAB_EXTENSION;
							rootEntity->m_hasPrefab = true;
							rootEntity->m_prefabPath = prefabPath;
							const std::string prefabString = Serializer::SerializePrefab(*rootEntity);
							SaveFile(prefabPath, prefabString);

							const std::string separator = "\\";
							const std::string path = DEFAULT_PREFAB_PATH;
							const std::string_view relativePath(path.c_str(), path.size() - 1);
							m_contentBrowser->DidFolderChange(relativePath);
						}
						ImGui::PopItemWidth();
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(104.f / 255.f, 151.f / 255.f, 227.f / 255.f, 1.f));
						if (ImGui::Button("Apply to prefab", ImVec2(ImGui::GetWindowContentRegionWidth(), 20)))
						{
							std::string newPrefabData = Serializer::SerializePrefab(*rootEntity);
							PrefabUtility::UpdateClones(rootEntity->GetID(), rootEntity->GetPrefabPath(), newPrefabData);
							SaveFile(rootEntity->GetPrefabPath(), newPrefabData);

							PrefabUtility::UpdatePrefabResource(rootEntity->GetPrefabPath());
						}
						ImGui::PopStyleColor();
					}
				}
			}
			ImGui::EndChild();
		}
	}

	bool ComponentEditor::DrawUIObject(const rttr::type& a_type, const rttr::instance& a_obj, int a_index, bool doSerialize) const
	{
		ImGui::PushID(a_index);
		bool hasChanged = false;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
		for (auto prop : a_type.get_properties())
		{
			const auto isTransform = a_obj.try_convert<Transform>();
			if (isTransform != nullptr)
			{
				if ((prop.get_name().to_string() == S_TRANSFORM_PROP_ROTATION && isTransform->GetParent() != nullptr) ||
					(prop.get_name().to_string() == S_TRANSFORM_PROP_LOCAL_ROTATION && isTransform->GetParent() == nullptr) ||
					(prop.get_name().to_string() == S_TRANSFORM_PROP_POS && isTransform->GetParent() != nullptr) ||
					(prop.get_name().to_string() == S_TRANSFORM_PROP_LOCAL_POS && isTransform->GetParent() == nullptr))
				{
					continue;
				}
			}

			a_index++;
			const auto hideInInspector = prop.get_metadata(HIDE_FROM_INSPECTOR);

			if (!hideInInspector)
			{
				rttr::variant var = prop.get_type().is_wrapper() ? prop.get_value(a_obj).extract_wrapped_value() : prop.get_value(a_obj);

				//Enumeration
				if (var.get_type().is_enumeration())
				{
					if (GenerateUIForEnumeration(a_obj, prop, a_index,doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//Array
				else if (var.is_sequential_container())
				{
					if (GenerateUIForArray(a_obj, prop, a_index,doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//Integer
				else if (var.is_type<int>())
				{
					if (prop.is_readonly())
					{
						const int val = var.get_value<int>();
						ImGui::Text("%i", val);
					}
					else
					{
						if (GenerateUIElementBasedOnType<int>(a_obj, prop, a_index,doSerialize))
						{
							hasChanged = true;
						}
					}
					ImGui::Separator();
				}
				//Boolean
				else if (var.is_type<bool>())
				{
					if (GenerateUIElementBasedOnType<bool>(a_obj, prop, a_index, doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//Float
				else if (var.is_type<float>())
				{
					if (GenerateUIElementBasedOnType<float>(a_obj, prop, a_index, doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//String
				else if (var.is_type<std::string>())
				{
					std::string myString = var.get_value<std::string>();
					char* charPointer = &myString[0];

					if (prop.is_readonly())
					{
						ImGui::Text("%s", charPointer);
					}
					else
					{
						if (GenerateUIForString(prop.get_name().to_string(), charPointer, a_obj, prop, a_index,doSerialize))
						{
							hasChanged = true;
						}
					}
					ImGui::Separator();
				}
				//Vector 2
				else if (var.is_type<Vec2>())
				{
					if (GenerateUIElementVector2(a_obj, prop, a_index,doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//Vector 3
				else if (var.is_type<Vec3>())
				{
					if (GenerateUIElementVector3(a_obj, prop, a_index,doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//Quaternion
				else if (var.is_type<Quaternion>())
				{
					if (GenerateUIElementQuaternion(a_obj, prop, a_index,doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//DX12 Texture
				else if (var.is_type<DX12Texture*>())
				{
					if (GenerateUIForResource<DX12Texture>(a_obj, prop, { ".png", ".jpg" }, a_index,doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//Audio asset
				else if (var.is_type<AudioAsset>())
				{
					if (GenerateUIForAudio(a_obj, prop, a_index,doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//Prefab
				else if (var.is_type<Prefab*>())
				{
					if (GenerateUIForResource<Prefab>(a_obj, prop, { ".prefab" }, a_index,doSerialize))
					{
						hasChanged = true;
					}
					ImGui::Separator();
				}
				//MeshAsset
				else if (var.is_type<MeshAsset>())
				{
					GenerateUIForMesh(a_obj, prop, a_index,doSerialize);
					ImGui::Separator();

				}
			}
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		return hasChanged;
	}

	bool ComponentEditor::GenerateUIForString(const std::string& a_label, const char* a_charPointer, const rttr::instance& a_obj, rttr::property& a_property, int a_index, bool a_doSerialize) const
	{
		ImGui::PushID(a_index);
		static const std::size_t MAX_CHARACTERS = 128;
		char bufferSize[MAX_CHARACTERS] = "";
		const std::size_t len = strlen(a_charPointer);
		HE_CORE_ASSERT(len < MAX_CHARACTERS, "Reached max character limit!");
		for (std::size_t i = 0; i < len; i++)
		{
			bufferSize[i] = a_charPointer[i];
		}

		if (!a_label.empty())
		{
			ImGui::Text("%s", a_label.c_str());
		}
		ImGui::InputText("##value", bufferSize, IM_ARRAYSIZE(bufferSize));

		std::string newString(bufferSize);
		rttr::variant varProp = a_property.get_value(a_obj);

		const std::string oldString = varProp.get_value<std::string>();
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			if (oldString != newString)
			{
				const std::string oldValue = Serializer::Serialize(a_obj);
				a_property.set_value(a_obj, newString);
				const std::string newValue = Serializer::Serialize(a_obj);
				if (a_doSerialize)
				{
					EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
				}
				ImGui::PopID();
				return true;
			}
		}
		ImGui::PopID();
		return false;
	}

	void ComponentEditor::GenerateUIForExposedLuaVariables(const rttr::instance& a_obj) const
	{
		auto lua = a_obj.try_convert<LuaComponent>();
		if (lua != nullptr)
		{
			for (auto p : lua->GetExposedLuaVariables())
			{
				if (p.second.is_type<double>())
				{
					const auto f = reinterpret_cast<float*>(&p.second.get_value<double>());
					ImGui::Text(p.first.c_str());
					ImGui::InputFloat("##float", f, 0.01f, 1.0f, "%.3f");
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						if (*f != static_cast<float>(p.second.get_value<double>()))
						{
							lua->SetExposedVariable(p.first, static_cast<double>(*f));
						}
					}
				}
				else if (p.second.is_type<int>())
				{
					const auto i = reinterpret_cast<int*>(&p.second.get_value<int>());
					ImGui::Text(p.first.c_str());
					ImGui::InputInt("##int", i, 1, 100);
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						if (*i != static_cast<int>(p.second.get_value<int>()))
						{
							lua->SetExposedVariable(p.first, static_cast<int>(*i));

						}
					}
				}
			}
		}
	}

	bool ComponentEditor::GenerateUIForEnumeration(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool a_doSerialize) const
	{
		const rttr::enumeration enumeration = a_property.get_enumeration();
		if (enumeration.get_names().size() > 1)
		{
			ImGui::PushID(a_index);
			ImGui::Text("%s", a_property.get_name().to_string().c_str());

			static std::string savedChoice = enumeration.get_names().begin()->to_string();
			if (a_property.get_value(a_obj) != nullptr)
			{
				savedChoice = a_property.get_value(a_obj).to_string();
			}

			if (ImGui::BeginCombo("Enumeration", savedChoice.c_str()))
			{
				bool changed = false;
				for (auto& e : enumeration.get_names())
				{
					const bool isSelected = (savedChoice == e);
					if (ImGui::Selectable(e.to_string().c_str(), isSelected))
					{
						savedChoice = e.to_string();
						changed = true;
					}
					if (isSelected)
					{
						// Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
				if (changed)
				{
					std::string oldValue;
					if (a_doSerialize)
					{
						oldValue = Serializer::Serialize(a_obj);
					}
					a_property.set_value(a_obj, enumeration.name_to_value(savedChoice));
					if (a_doSerialize)
					{
						std::string newValue = Serializer::Serialize(a_obj);
						EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
					}
					ImGui::PopID();
					return true;
				}
			}

			ImGui::PopID();
		}
		return false;
	}

	bool ComponentEditor::GenerateUIElementVector2(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool doSerialze) const
	{
		ImGui::PushID(a_index);

		rttr::variant varProp = a_property.get_value(a_obj);

		const Vec2 v2 = varProp.get_value<Vec2>();
		ImGui::Text("%s", a_property.get_name().to_string().c_str());
		float array1[2] = { v2.x,v2.y };
		float array2[2] = { v2.x,v2.y };

		ImGui::InputFloat2("X|Y", array1, 3);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			const Vec2 old = Vec2(array2[0], array2[1]);
			Vec2 current = Vec2(array1[0], array1[1]);

			if (current != old)
			{
				const std::string oldValue = Serializer::Serialize(a_obj, true);
				a_property.set_value(a_obj, current);
				const std::string newValue = Serializer::Serialize(a_obj, true);
				if (doSerialze)
				{
					EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
				}

				ImGui::PopID();
				return true;
			}

		}

		ImGui::PopID();
		return false;
	}

	bool ComponentEditor::GenerateUIElementVector3(const rttr::instance& a_obj, const rttr::property& a_prop, int a_index, bool a_doSerialize) const
	{
		ImGui::PushID(a_index);

		rttr::variant varProp = a_prop.get_value(a_obj);


		const Vec3 v3 = varProp.get_value<Vec3>();
		ImGui::Text("%s", a_prop.get_name().to_string().c_str());
		float array1[3] = { v3.x,v3.y, v3.z };
		float array2[3] = { v3.x,v3.y, v3.z };

		ImGui::InputFloat3("#X|Y|Z", array1, 3);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			const Vec3 old = Vec3(array2[0], array2[1], array2[2]);
			Vec3 current = Vec3(array1[0], array1[1], array1[2]);

			if (current != old)
			{
				const std::string oldValue = Serializer::Serialize(a_obj, true);
				a_prop.set_value(a_obj, current);
				const std::string newValue = Serializer::Serialize(a_obj, true);
				if (a_doSerialize)
				{
					EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
				};

				ImGui::PopID();
				return true;
			}
		}

		ImGui::PopID();
		return false;
	}

	bool ComponentEditor::GenerateUIElementQuaternion(const rttr::instance& a_obj, const rttr::property& a_prop, int a_index, bool a_doSerialize) const
	{
		ImGui::PushID(a_index);

		rttr::variant varProp = a_prop.get_value(a_obj);
		const Quaternion quat = varProp.get_value<Quaternion>();
		ImGui::Text("%s", a_prop.get_name().to_string().c_str());
		const Vec3 euler = glm::degrees(glm::eulerAngles(quat));
		float currentVal[3] = { euler.x, euler.y, euler.z };

		ImGui::InputFloat3("#X|Y|Z", currentVal, 2);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			const Vec3 current = Vec3(currentVal[0], currentVal[1], currentVal[2]);

			//Check if new value is not the same as old (euler).
			if (current != euler)
			{
				const std::string oldValue = Serializer::Serialize(a_obj, true);
				a_prop.set_value(a_obj, Quaternion(glm::radians(current)));
				const std::string newValue = Serializer::Serialize(a_obj, true);
				if (a_doSerialize)
				{
					EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
				}

				ImGui::PopID();
				return true;
			}
		}

		ImGui::PopID();
		return false;
	}

	bool ComponentEditor::GenerateUIForAudio(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool doSerialize) const
	{
		ImGui::PushID(a_index);
		ImGui::Text("%s", a_property.get_name().to_string().c_str());
		std::vector<FileData> files = { FileData() };
		std::vector<FileData> filesWithExtension = m_contentBrowser->GetAllFilesOfExtensions({ ".mp3", ".wav" });
		files.insert(files.end(), filesWithExtension.begin(), filesWithExtension.end());

		static FileData curFile = files[0];

		rttr::variant var = a_property.get_value(a_obj);

		std::string currentFilePath;
		if (var.is_type<AudioAsset>())
		{
			currentFilePath = var.get_value<AudioAsset>().m_filePath;
		}
		if (currentFilePath.empty())
		{
			currentFilePath = file_data::NONE_FILE;
		}

		if (ImGui::BeginCombo("Audio Files", currentFilePath.c_str()))
		{
			bool changed = false;
			for (auto& f : files)
			{
				const bool isSelected = (curFile.m_fileName == f.m_fileName);
				if (ImGui::Selectable(f.m_fileName.c_str(), isSelected))
				{
					changed = true;
					curFile = f;
				}
				if (isSelected)
				{
					// Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
					ImGui::SetItemDefaultFocus();
				}

			}
			ImGui::EndCombo();
			if (changed)
			{
				if (curFile.m_fileName != file_data::NONE_FILE)
				{
					std::string oldValue = Serializer::Serialize(a_obj);
					//The load the correct resource.
					AudioAsset asset;
					asset.m_filePath = curFile.m_filePath;
					a_property.set_value(a_obj, asset);
					std::string newValue = Serializer::Serialize(a_obj);
					if (doSerialize)
					{
						EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
					}
				}
				else
				{
					std::string oldValue = Serializer::Serialize(a_obj);
					AudioAsset asset;
					asset.m_filePath = "";
					a_property.set_value(a_obj, asset);
					std::string newValue = Serializer::Serialize(a_obj);
					EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
				}

				ImGui::PopID();
				return true;
			}

		}
		if (ImGui::Button("Play Sound"))
		{
			auto source = a_obj.try_convert<AudioSource>();
			if (source != nullptr)
			{
				source->Play();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop Sound"))
		{
			auto source = a_obj.try_convert<AudioSource>();
			if (source != nullptr)
			{
				source->Stop();
			}
		}
		ImGui::PopID();
		return false;
	}

	bool ComponentEditor::GenerateUIForArray(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool a_doSerialize) const
	{
		ImGui::Indent();
		ImGui::PushID(a_index);
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.65f, 0.65f, 0.65f, 1.f));
		if (ImGui::CollapsingHeader((a_property.get_name().to_string() + " (Array)").c_str()))
		{
			bool changed = false;
			const rttr::variant& arrProp = a_property.get_value(a_obj);
			rttr::variant_sequential_view view = arrProp.create_sequential_view();

			//Swap an item with a previous item in the array or a next item.
			const std::function<void(rttr::variant_sequential_view&, rttr::variant&, std::size_t, std::size_t)> swapItems =
				[](auto& a_view, auto& a_itemA, std::size_t a_fomIndex, std::size_t a_toIndex)
			{
				const bool isWrapper = a_view.get_value(a_fomIndex).get_type().is_wrapper();
				auto itemB = isWrapper ? a_view.get_value(a_toIndex).extract_wrapped_value() : a_view.get_value(a_toIndex);

				a_view.set_value(a_fomIndex, itemB);
				a_view.set_value(a_toIndex, a_itemA);
			};

			const std::size_t size = view.get_size();
			for (std::size_t i = 0; i < size; ++i)
			{
				a_index++;
				const bool isWrapper = view.get_value(i).get_type().is_wrapper();
				auto item = isWrapper ? view.get_value(i).extract_wrapped_value() : view.get_value(i);

				ImGui::Text("Index: %i", i);
				ImGui::SameLine();
				//Swap items up or down.
				if (i > 0)
				{
					ImGui::PushID(std::to_string(a_index + i).c_str());
					if (ImGui::SmallButton(ICON_FA_ANGLE_UP))
					{
						swapItems(view, item, i, i - 1);
						changed = true;
						ImGui::PopID();
						continue;
					}
					ImGui::PopID();

					if (i < size - 1)
					{
						ImGui::SameLine();
					}
				}
				if (i < size - 1)
				{
					ImGui::PushID(std::to_string(a_index + i + 1).c_str());
					if (ImGui::SmallButton(ICON_FA_ANGLE_DOWN))
					{
						swapItems(view, item, i, i + 1);
						changed = true;
						ImGui::PopID();
						continue;
					}
					ImGui::PopID();
				}

				ImGui::Indent();
				//Display correct GUI for item.
				if (item.get_type().is_arithmetic())
				{
					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
					if (item.is_type<int>())
					{
						int value = item.get_value<int>();
						GenerateInputField<int>("", item, value, a_index);

						if (value != item.get_value<int>())
						{
							view.set_value(i, value);
							changed = true;
						}
						ImGui::Separator();
					}
					else if (item.is_type<float>())
					{
						float value = item.get_value<float>();
						GenerateInputField<float>("", item, value, a_index);

						if (value != item.get_value<float>())
						{
							view.set_value(i, value);
							changed = true;
						}
						ImGui::Separator();
					}
					else if (item.is_type<bool>())
					{
						bool value = item.get_value<bool>();
						GenerateInputField<bool>("", item, value, a_index);

						if (value != item.get_value<bool>())
						{
							view.set_value(i, value);
							changed = true;
						}
						ImGui::Separator();
					}
					ImGui::PopItemWidth();
				}
				else if (!item.get_type().get_properties().empty())
				{
					if (DrawUIObject(item.get_type(), item, a_index,false))
					{
						changed = true;
						view.set_value(i, item);
					}
				}
				else
				{
					HE_CORE_ASSERT(false, "Value inside array is not supported by the editor.");
				}
				ImGui::Unindent();
			}

			//Adding array element
			if (ImGui::Button(ICON_FA_PLUS))
			{
				if (view.get_value_type().is_arithmetic())
				{
					if (view.set_size(size + 1))
					{
						view.set_value(view.get_size() - 1, 0);
						changed = true;
					}
				}
				else
				{
					rttr::variant newItem = view.get_value_type().is_wrapper() ? view.get_value_type().get_wrapped_type().create() : view.get_value_type().create();
					if (view.set_size(size + 1))
					{
						view.set_value(view.get_size() - 1, newItem);
						changed = true;
					}
				}
			}
			//Removing array element
			if (view.get_size() > 0)
			{
				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_MINUS))
				{
					if (view.set_size(view.get_size() - 1))
					{
						changed = true;
					}
				}
			}

			//Save the array if anything has changed
			if (changed)
			{
				const float currentScrollY = ImGui::GetScrollY();

				//bool isComponent = a_obj.try_convert<Component>();
				
				std::string oldValue;
				if (a_doSerialize)
				{
					oldValue = Serializer::Serialize(a_obj, true);
				}

				if (!a_property.set_value(a_obj, arrProp))
				{
					HE_CORE_CRITICAL("Couldn't set array value!");
				}
				ImGui::SetScrollY(currentScrollY);

				if (a_doSerialize)
				{
					const std::string newValue = Serializer::Serialize(a_obj, true);
					EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
				}


				
				ImGui::PopID();
				ImGui::Unindent();
				ImGui::PopStyleColor();
				return true;
			}
		}
		ImGui::PopID();
		ImGui::Unindent();
		ImGui::PopStyleColor();
		return false;
	}

	void ComponentEditor::GenerateUIForMesh(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool doSerialize) const
	{
		ImGui::PushID(a_index);
		ImGui::Text("%s", a_property.get_name().to_string().c_str());
		std::vector<FileData> files = { FileData() };
		std::vector<FileData> filesWithExtension = m_contentBrowser->GetAllFilesOfExtensions({ ".gltf" });
		files.insert(files.end(), filesWithExtension.begin(), filesWithExtension.end());

		static FileData curFile = files[0];

		rttr::variant var = a_property.get_value(a_obj);

		std::string currentFilePath;
		if (var.is_type<MeshAsset>())
		{
			currentFilePath = var.get_value<MeshAsset>().m_filepath;
		}
		if (currentFilePath.empty())
		{
			currentFilePath = file_data::NONE_FILE;
		}

		if (ImGui::BeginCombo("GLTF Files", currentFilePath.c_str()))
		{
			bool changed = false;
			for (auto& f : files)
			{
				const bool isSelected = (curFile.m_fileName == f.m_fileName);
				if (ImGui::Selectable(f.m_fileName.c_str(), isSelected))
				{
					changed = true;
					curFile = f;
				}
				if (isSelected)
				{
					// Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
					ImGui::SetItemDefaultFocus();
				}

			}
			ImGui::EndCombo();
			if (changed)
			{
				if (curFile.m_fileName != file_data::NONE_FILE)
				{
					std::string oldValue = Serializer::Serialize(a_obj);
					//The load the correct resource.
					MeshAsset asset;
					asset.m_filepath = curFile.m_filePath;
					a_property.set_value(a_obj, asset);
					std::string newValue = Serializer::Serialize(a_obj);
					MeshRenderer* mr = a_obj.try_convert<MeshRenderer>();
					mr->LoadMeshFromPath(asset.m_filepath);
					if (doSerialize)
					{
						EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
					}
				}
				else
				{
					std::string oldValue = Serializer::Serialize(a_obj);
					MeshAsset asset;
					asset.m_filepath = "";
					a_property.set_value(a_obj, asset);
					std::string newValue = Serializer::Serialize(a_obj);
					EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
				}

			}

		}
		ImGui::PopID();

	}
}