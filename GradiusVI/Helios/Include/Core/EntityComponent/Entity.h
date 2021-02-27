#pragma once
#include "Core/Core.h"
#include "Component.h"
#include "Components/RenderComponent.h"
#include "Components/ScriptableComponent.h"
#include "Components/Colliders/Collider.h"
#include "Core/Logger.h"

namespace Helios
{
    class Prefab;

	class Entity final
	{
		friend class EditorHistory;
		friend class ComponentEditor;
		friend class EntityManager;
		friend class Deserializer;
		
		RTTR_ENABLE()
	public:
		Entity(EntityID a_id, std::size_t a_index, std::string name);
		~Entity() = default;

		const EntityID& GetID() const { return m_entityID; }
		std::size_t GetIndex() const { return m_index; }
		std::string GetName() const { return m_name; }
        Transform& GetTransform() const { return *m_transform; }

		bool IsEnabled() const { return m_isEnabled; }
		void SetEnabled(bool a_enabled);
		
		bool HasPrefab() const { return m_hasPrefab; }
		const std::string& GetPrefabPath() const { return m_prefabPath; }

		void Update(float a_deltaTime);

		template <typename T, typename = typename std::enable_if<std::is_base_of<Component, T>::value, T>::type>
		T* AddComponent();
		Component* AddComponent(std::unique_ptr<Component>&& a_comp);

        template <typename T, typename = typename std::enable_if<std::is_base_of<Component, T>::value, T>::type>
		T* GetComponent();
	
        template <typename T, typename = typename std::enable_if<std::is_base_of<Component, T>::value, T>::type>
        std::vector<T*> GetComponentsByType();

        std::vector<Component*> GetAllComponents();
        Component* GetComponentByID(const ComponentID& a_id) const;

		void RemoveComponent(Component* a_component);
		
		std::size_t GetComponentIDCount() const { return m_componentIDs; }
		std::size_t GetComponentCount() const { return m_components.size(); }

		std::vector<RenderComponent*>& GetRenderComponents();
		std::vector<ScriptableComponent*>& GetScriptableComponents();
	private:
		EntityID m_entityID;
		std::size_t m_index;
		std::string m_name;
		bool m_isLoaded;
		bool m_isEnabled;

		//Prefab data
		bool m_hasPrefab;
		std::string m_prefabPath;

		std::vector<std::unique_ptr<Component>> m_components;
		std::vector<RenderComponent*> m_renderComponents;
		std::vector<ScriptableComponent*> m_scriptableComponents;

        Transform* m_transform;
		std::size_t m_componentIDs;

		void SetEntityID(const EntityID& a_id) { m_entityID = a_id; }

		RTTR_REGISTRATION_FRIEND
	};

	template <typename T, typename>
	T* Entity::AddComponent()
	{
		std::unique_ptr<Component> comp = std::make_unique<T>(*this, ComponentID{ m_componentIDs });
		return dynamic_cast<T*>(AddComponent(std::move(comp)));
	}

    template <typename T, typename>
	T* Entity::GetComponent()
	{
		auto it = std::find_if(m_components.begin(), m_components.end(), [](const auto& a_comp)
		{
			return dynamic_cast<T*>(a_comp.get()) != nullptr;
		});

		if (it != m_components.end())
		{
			return dynamic_cast<T*>((*it).get());
		}

		HE_CORE_ERROR("Cannot find component of type({}) on entity({})!", rttr::type::get<T>().get_name(), m_name);
		return nullptr;
	}

    template <typename T, typename>
    std::vector<T*> Entity::GetComponentsByType()
    {
        std::vector<T*> components;

        for(auto &it : m_components)
        {
        	T* c = dynamic_cast<T*>(it.get());
            if(c != nullptr)
            {
                components.push_back(c);
            }
        }

        return components;
    }
}
