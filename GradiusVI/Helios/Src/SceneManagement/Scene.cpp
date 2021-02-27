#include "hepch.h"
#include "SceneManagement/Scene.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Core/CoreSerializationNames.h"

namespace Helios
{
	Scene::Scene(std::string a_sceneName)
		: m_sceneName(std::move(a_sceneName)), m_scenePath(""), m_isLoaded(false), m_isValid(false)
	{

	}

	void Scene::AddEntity(Entity* a_entity)
	{
		m_entitiesInScene.push_back(a_entity);
	}

	void Scene::RemoveEntity(Entity* a_entity)
	{
		m_entitiesInScene.erase(std::find(m_entitiesInScene.begin(), m_entitiesInScene.end(), a_entity));
	}

	void Scene::SetSelectedEntity(Entity* a_entity)
	{
		m_selectedEntity = a_entity;
	}

	Entity* Scene::GetSelectedEntity() const
	{
		return m_selectedEntity;
	}
}

RTTR_REGISTRATION
{
    RTTR_REGISTER_CLASS(Helios::Scene, S_SCENE_NAME, std::string)
    RTTR_PROPERTY(S_SCENE_PROP_NAME, Helios::Scene::m_sceneName)
    RTTR_PROPERTY(S_SCENE_PROP_PATH, Helios::Scene::m_scenePath)
    RTTR_PROPERTY_READ_ONLY(S_SCENE_PROP_ENTITIES, Helios::Scene::m_entitiesInScene);
}