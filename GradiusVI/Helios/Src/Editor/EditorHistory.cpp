#include "hepch.h"
#include "Editor/EditorHistory.h"
#include "Core/EntityComponent/EntityManager.h"
#include "SceneManagement/SceneManager.h"
#include "Utils/Serializer.h"
#include "Core/CoreSerializationNames.h"

std::deque<Helios::EditorHistory::EditorChange> Helios::EditorHistory::m_undoList = std::deque<EditorChange>();
std::deque<Helios::EditorHistory::EditorChange> Helios::EditorHistory::m_redoList = std::deque<EditorChange>();

namespace Helios
{
	constexpr auto MAX_HISTORY = 3;

	void EditorHistory::AddChange(EditorChange::EChangeType a_type, const std::string& a_oldValue, const std::string& a_newValue)
	{
        if(m_redoList.size() >= MAX_HISTORY)
        {
            m_redoList.pop_back();
        }

		//Check if our undo list is bigger than the max history if so remove the earliest change.
		//Do the same if we have changes in the undo list and redo list.
		//if (m_undoList.size() >= MAX_HISTORY ||
		//	(!m_undoList.empty() && m_undoList.size() + m_redoList.size() >= MAX_HISTORY))
		//{
		//	m_undoList.pop_front();
		//}
		////If we have no changes in the undo list but our undo list reached the max history remove the earliest item.
		//else if (m_redoList.size() >= MAX_HISTORY)
		//{
		//	m_redoList.pop_back();
		//}

		EditorChange editorChange;
		editorChange.m_changeType = a_type;
		editorChange.m_oldValue = a_oldValue;
		editorChange.m_newValue = a_newValue;
		m_undoList.push_back(editorChange);
	}

	void EditorHistory::UndoChange()
	{
		if (!m_undoList.empty())
		{
			const EditorChange change = m_undoList.back();

			if (change.m_changeType == EditorChange::EChangeType::CHANGE_TYPE_COMPONENT)
			{
				if (!CanModifyComponent(change.m_oldValue))
				{
					RemoveComponent(change.m_newValue);
				}
			}
			else if (change.m_changeType == EditorChange::EChangeType::CHANGE_TYPE_ENTITY)
			{
				if (!CanModifyEntity(change.m_oldValue))
				{
					RemoveEntity(change.m_newValue);
				}
			}

			m_redoList.push_back(change);
			m_undoList.pop_back();
		}
	}

	void EditorHistory::RedoChange()
	{
		if (!m_redoList.empty())
		{
			const EditorChange change = m_redoList.back();

			if (change.m_changeType == EditorChange::EChangeType::CHANGE_TYPE_COMPONENT)
			{
				if (!CanModifyComponent(change.m_newValue))
				{
					RemoveComponent(change.m_oldValue);
				}
			}
			else if (change.m_changeType == EditorChange::EChangeType::CHANGE_TYPE_ENTITY)
			{
				if (!CanModifyEntity(change.m_newValue))
				{
					RemoveEntity(change.m_oldValue);
				}
			}

			m_undoList.push_back(change);
			m_redoList.pop_back();
		}
	}
	bool EditorHistory::CanModifyComponent(const std::string& a_inputString)
	{
		//If its empty it means from a redo or undo we remove the component so we return false
		if (!a_inputString.empty())
		{
			rapidjson::Document doc;
			if (doc.Parse(a_inputString.c_str()).HasParseError())
			{
				HE_CORE_WARN("Parsing error!");
				return false;
			}
			const std::string strType = doc.FindMember(S_TYPE)->value.GetString();

			const EntityID entityId
			{
				std::size_t(doc.FindMember(S_COMPONENT_PROP_OWNER_ID)->value.GetObjectW().FindMember(S_ENTITYID_PROP_ID)->value.GetInt())
			};

			const ComponentID componentID
			{
				std::size_t(doc.FindMember(S_COMPONENT_PROP_ID)->value.GetObjectW().FindMember(S_COMPONENTID_PROP_ID)->value.GetInt())
			};

			Component* c = EntityManager::GetInstance().GetEntityByUniqueId(entityId)->GetComponentByID(componentID);
			//The component already exists Hooray! just change values.
			if (c != nullptr)
			{
				Deserializer::Deserialize(doc, *c);
			}
			//It doesn't exists, therefore we create a new component with the same original ID
			else
			{
				const rttr::type classType = rttr::type::get_by_name(strType);
				rttr::variant obj = classType.create({ static_cast<Entity&>(*EntityManager::GetInstance().GetEntityByUniqueId(entityId)), componentID });

				Deserializer::Deserialize(doc, obj);

				EntityManager::GetInstance().GetEntityByUniqueId(entityId)->AddComponent(std::unique_ptr<Component>(obj.get_value<Component*>()));
			}
			return true;
		}
		return false;
	}

	void EditorHistory::RemoveComponent(const std::string& a_inputString)
	{
		if (!a_inputString.empty())
		{
			rapidjson::Document doc;
			if (doc.Parse(a_inputString.c_str()).HasParseError())
			{
				HE_CORE_WARN("Parsing error!");
				return;
			}

			const ComponentID componentID
			{
				std::size_t(doc.FindMember(S_COMPONENT_PROP_ID)->value.GetObjectW().FindMember(S_COMPONENTID_PROP_ID)->value.GetInt())
			};

			const EntityID entityId
			{
				std::size_t(doc.FindMember(S_COMPONENT_PROP_OWNER_ID)->value.GetObjectW().FindMember(S_ENTITYID_PROP_ID)->value.GetInt())
			};
			Component* toRemove = EntityManager::GetInstance().GetEntityByUniqueId(entityId)->GetComponentByID(componentID);
			EntityManager::GetInstance().GetEntityByUniqueId(entityId)->RemoveComponent(toRemove);
		}
	}

	bool EditorHistory::CanModifyEntity(const std::string& a_inputString)
	{
		bool modified = false;

		//Re-add the entity, Or modify Values if it already exists
		if (!a_inputString.empty())
		{
			rapidjson::Document doc;
			if (doc.Parse(a_inputString.c_str()).HasParseError())
			{
				HE_CORE_WARN("Parsing error!");
				return false;
			}

			const std::string type = doc.FindMember(S_TYPE)->value.GetString();
			if (type == S_ENTITY_NAME)
			{
				//First find the unique ID in the serialized string and check if it already exists.
				const std::size_t uniqueId = doc.FindMember(S_ENTITY_PROP_ENTITY_ID)->value.GetObjectW().FindMember(S_ENTITYID_PROP_ID)->value.GetInt();
				const EntityID entityId(uniqueId);
				Entity* foundEntity = EntityManager::GetInstance().GetEntityByUniqueId(entityId);

				if (foundEntity == nullptr)
				{
					//This means we have to create a new entity...
					const std::string entityName = doc.FindMember(S_ENTITY_PROP_NAME)->value.GetString();
					Entity* entity = EntityManager::GetInstance().CreateEntityDirectly(entityName, SceneManager::GetInstance().GetActiveScene(), false);
					Deserializer::DeserializeEntity(*entity, doc);
					entity->SetEntityID(entityId);

					modified = true;
				}
				else
				{
					//It exists! There for we want to only change a or some values.
					const std::string oldName = doc.FindMember(S_ENTITY_PROP_NAME)->value.GetString();
					const rttr::type e = rttr::type::get(*foundEntity);

					const rttr::property prop = e.get_property(S_ENTITY_PROP_NAME);
					prop.set_value(*foundEntity, oldName);
					modified = true;
				}
			}
			else
			{
				const auto entities = doc.FindMember(S_SCENE_PROP_ENTITIES)->value.GetArray();

				const std::string prefabPath = entities[0].FindMember(S_ENTITY_PROP_PREFAB_PATH)->value.GetString();
				Deserializer::DeserializePrefab(doc, prefabPath, true, true);

				modified = true;
			}
		}
		return modified;
	}

	void EditorHistory::RemoveEntity(const std::string& inputString)
	{
		if (!inputString.empty())
		{
			rapidjson::Document doc;
			if (doc.Parse(inputString.c_str()).HasParseError())
			{
				HE_CORE_WARN("Parsing error!");
				return;
			}

			const std::string type = doc.FindMember(S_TYPE)->value.GetString();

			EntityID entityId;
			if (type == S_ENTITY_NAME)
			{
				entityId = EntityID(std::size_t(doc.FindMember(S_ENTITY_PROP_ENTITY_ID)->value.GetObjectW().FindMember(S_ENTITYID_PROP_ID)->value.GetInt()));
			}
			else
			{
				entityId = EntityID(std::size_t(doc.FindMember(S_SCENE_PROP_ENTITIES)->value.GetArray()[0].
					FindMember(S_ENTITY_PROP_ENTITY_ID)->value.GetObjectW().
					FindMember(S_ENTITYID_PROP_ID)->value.GetInt()));
			}

			Entity* e = EntityManager::GetInstance().GetEntityByUniqueId(entityId);
			EntityManager::GetInstance().DestroyEntityDirectly(e, SceneManager::GetInstance().GetActiveScene());
		}
	}

	void EditorHistory::ClearHistory()
	{
		m_redoList.clear();
		m_undoList.clear();
	}
}

