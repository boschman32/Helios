#pragma once
#pragma once
#include "EditorWindow.h"
#include "Editor/EditorHistory.h"
#include "Core/EntityComponent/Component.h"

#include "Utils/Serializer.h"
#include "Core/ResourceManager.h"
#include "Editor/ContentBrowser.h"

namespace Helios
{
	class Entity;
	class ComponentEditor : public EditorWindow
	{
	public:
		ComponentEditor(const std::string& a_windowName, float a_xPos, float a_yPos, float a_xSize, float a_ySize, ContentBrowser& a_contentBrowser);
		virtual ~ComponentEditor() = default;

		void ShowContext() override;

		bool DrawUIObject(const rttr::type& a_type, const rttr::instance& a_obj, int a_index, bool doSerialize = true) const;

		bool GenerateUIForString(const std::string& a_label, const char* a_charPointer, const rttr::instance& a_obj, rttr::property& a_property, int
		                         a_index, bool a_doSerialize = true) const;
		bool GenerateUIForEnumeration(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool a_doSerialize = true) const;
		bool GenerateUIElementVector2(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool a_doSerialize = true) const;
		bool GenerateUIElementVector3(const rttr::instance& a_obj, const rttr::property& a_prop, int a_index, bool a_doSerialize = true) const;
		bool GenerateUIElementQuaternion(const rttr::instance& a_obj, const rttr::property& a_prop, int a_index, bool a_doSerialize = true) const;
		bool GenerateUIForAudio(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool a_doSerialize = true) const;
		bool GenerateUIForArray(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool a_doSerialize = true) const;

		template<typename T>
		bool GenerateUIElementBasedOnType(const rttr::instance& a_obj, rttr::property& a_property, int a_index, bool a_doSerialize = false) const;
		template<typename T>
		bool GenerateInputField(const std::string& a_name, const rttr::variant& a_obj, T& a_var, int a_index) const;
		template<typename T>
		bool GenerateUIForResource(const rttr::instance& a_obj, const rttr::property& a_property, const std::vector<std::string>& a_extensions, int a_index, bool a_doSerialize) const;
		void GenerateUIForExposedLuaVariables(const rttr::instance& a_obj) const;

		void GenerateUIForMesh(const rttr::instance& a_obj, const rttr::property& a_property, int a_index, bool doSerialize = true) const;
	private:
		ContentBrowser* m_contentBrowser;
	};

	template<typename T>
	bool ComponentEditor::GenerateUIElementBasedOnType(const rttr::instance& a_obj, rttr::property& a_property, int a_index, bool a_doSerialize) const
	{
		rttr::variant var = a_property.get_value(a_obj);
		T p = var.get_value<T>();
		const std::string name = a_property.get_name().to_string();
		if (GenerateInputField<T>(name, var, p, a_index))
		{
			if (p != var.get_value<T>())
			{
				std::string oldValue;
				std::string newValue;
				if (a_doSerialize)
				{
					oldValue = Serializer::Serialize(a_obj);
				}
				a_property.set_value(a_obj, p);
				if (a_doSerialize)
				{
					newValue = Serializer::Serialize(a_obj);
					EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_COMPONENT, oldValue, newValue);
				}

				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool ComponentEditor::GenerateInputField(const std::string& a_name, const rttr::variant& a_obj, T& a_var, int a_index) const
	{
		ImGui::PushID(a_index);
		if (a_obj.is_type<int>())
		{
			auto i = reinterpret_cast<int*>(&a_var);
			ImGui::Text(a_name.c_str());
			ImGui::InputInt("##int", i, 1, 100);
		}
		else if (a_obj.is_type<float>())
		{
			auto f = reinterpret_cast<float*>(&a_var);
			ImGui::Text(a_name.c_str());
			ImGui::InputFloat("##float", f, 0.01f, 1.0f, "%.3f");
		}
		else if (a_obj.is_type<bool>())
		{
			auto b = reinterpret_cast<bool*>(&a_var);
			ImGui::Text(a_name.c_str());
			ImGui::Checkbox("##bool", b);
		}

		ImGui::PopID();

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			return true;
		}

		return false;
	}

	template <typename T>
	bool ComponentEditor::GenerateUIForResource(const rttr::instance& a_obj, const rttr::property& a_property, const std::vector<std::string>& a_extensions, int a_index, bool a_doSerialize) const
	{
		ImGui::PushID(a_index);
		ImGui::Text("%s", a_property.get_name().to_string().c_str());
		std::vector<FileData> files = { FileData() };
		std::vector<FileData> filesWithExtension = m_contentBrowser->GetAllFilesOfExtensions(a_extensions);
		files.insert(files.end(), filesWithExtension.begin(), filesWithExtension.end());

		static FileData curFile = files[0];

		//Check if we have already a texture loaded in if so we have selected that as our file.
		std::weak_ptr<T> loadedResource;
		std::string loadedFile;
		if (a_property.get_value(a_obj) != nullptr)
		{
			loadedResource = a_property.get_value(a_obj).get_value<std::weak_ptr<T>>();
			loadedFile = loadedResource.lock()->GetFileName();
		}
		else
		{
			loadedFile = files[0].m_fileName;
		}

		if (ImGui::BeginCombo("Prefabs", loadedFile.c_str()))
		{
			bool changed = false;

			for (auto& f : files)
			{

				const bool isSelected = (loadedFile == f.m_fileName);
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
				const std::string oldValue = Serializer::Serialize(a_obj);
				if (!loadedResource.expired() && loadedResource.lock()->GetPath() != curFile.m_filePath)
				{
					//First unload the previous resource if it has changed
					a_property.set_value(a_obj, std::shared_ptr<T>());
					ResourceManager::GetInstance().UnloadResource(loadedResource.lock());
				}

				if (curFile.m_fileName != file_data::NONE_FILE)
				{
					//Then load the correct resource.
					std::shared_ptr<T> r = ResourceManager::GetInstance().LoadResource<T>(curFile.m_filePath);
					a_property.set_value(a_obj, r);
				}
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

}