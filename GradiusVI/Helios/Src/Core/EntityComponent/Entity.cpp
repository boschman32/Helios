#include "hepch.h"
#include "Core/EntityComponent/Entity.h"
#include "Components/RenderComponent.h"
#include "Components/ScriptableComponent.h"
#include "Core/CoreSerializationNames.h"
#include "Components/Transform.h"
#include "Core/Core.h"
#include "Core/CollisionManager.h"
#include "Core/Application.h"

namespace Helios
{
	Entity::Entity(EntityID a_id, std::size_t a_index, std::string name)
		: m_entityID(std::move(a_id)),
		m_index(a_index),
		m_name(std::move(name)),
		m_isLoaded(false),
		m_isEnabled(true),
		m_hasPrefab(false),
		m_transform(nullptr),
		m_componentIDs(0)
	{

	}

	void Entity::Update(float a_deltaTime)
	{
		OPTICK_EVENT();
		if (!m_scriptableComponents.empty())
		{
			for (std::size_t i = 0; i < m_scriptableComponents.size(); ++i)
			{
				ScriptableComponent* script = m_scriptableComponents[i];
				if (script->IsEnabled())
				{
					script->Update(a_deltaTime);
				}
			}
		}
	}

	Component* Entity::AddComponent(std::unique_ptr<Component>&& a_comp)
	{
		//We can't have two transform components.
		HE_CORE_ASSERT((rttr::type::get(*a_comp.get()).get_raw_type().get_name() != S_TRANSFORM_NAME) != (m_transform == nullptr), "Can't add another transform component to this entity can only support one!");

		if (const auto t = dynamic_cast<Transform*>(a_comp.get()))
		{
			m_transform = t;
		}
		else if (const auto r = dynamic_cast<RenderComponent*>(a_comp.get()))
		{
			m_renderComponents.push_back(r);
		}
		else if (const auto s = dynamic_cast<ScriptableComponent*>(a_comp.get()))
		{
			m_scriptableComponents.push_back(s);
			if (m_isLoaded)
			{
				s->Start();
			}
		}
		else if (const auto c = dynamic_cast<Collider*>(a_comp.get()))
		{
			CollisionManager::GetInstance().AddCollider(c);
		}

		m_componentIDs = a_comp->GetID().Value() + 1;
		a_comp->OnCreate();
		m_components.push_back(std::move(a_comp));
		return m_components.back().get();
	}

	void Entity::RemoveComponent(Component* a_component)
	{
		//Check if the component even exists
		const auto it = std::find_if(m_components.begin(), m_components.end(), [&a_component](const auto& comp)
			{
				return comp.get() == a_component;
			});

		if (it != m_components.end())
		{
			//Check if this component is in any of the other lists.
			if (const auto r = dynamic_cast<RenderComponent*>(a_component))
			{
				m_renderComponents.erase(std::remove(m_renderComponents.begin(), m_renderComponents.end(), r), m_renderComponents.end());
			}
			else if (const auto s = dynamic_cast<ScriptableComponent*>(a_component))
			{
				m_scriptableComponents.erase(std::remove(m_scriptableComponents.begin(), m_scriptableComponents.end(), s), m_scriptableComponents.end());
			}
			else if (const auto c = dynamic_cast<Collider*>(a_component))
			{
				CollisionManager::GetInstance().RemoveCollider(c);
			}

			m_components.erase(it);
			return;
		}

		if (m_components.empty())
		{
			HE_CORE_CRITICAL("Removing component of type({}) from entity({}) while it has no components!", rttr::type::get(a_component).get_name(), m_name);
			HE_CORE_ASSERT("Removing components on an entity without any components!");
		}
		else
		{
			HE_CORE_ERROR("Cannot remove component of type({}) from entity({}) as this component does not exist!", rttr::type::get(a_component).get_name(), m_name);
		}
	}

	std::vector<Component*> Entity::GetAllComponents()
	{
		std::vector<Component*> components;
		for (auto& c : m_components)
		{
			components.push_back(c.get());
		}
		return components;
	}

	Component* Entity::GetComponentByID(const ComponentID& a_id) const
	{
		for (auto& c : m_components)
		{
			if (c->GetID() == a_id)
			{
				return c.get();
			}
		}
		return nullptr;
	}

	std::vector<RenderComponent*>& Entity::GetRenderComponents()
	{
		return m_renderComponents;
	}

	std::vector<ScriptableComponent*>& Entity::GetScriptableComponents()
	{
		return m_scriptableComponents;
	}

	void Entity::SetEnabled(bool a_enabled)
	{
		if(!m_isLoaded && !m_isEnabled)
		{
			for(auto s : m_scriptableComponents)
			{
				s->Start();
			}
			m_isLoaded = true;
		}
		m_isEnabled = a_enabled;
		
		//Disable or enable the children as well.
		for(auto c : m_transform->GetChildren())
		{
			c->GetOwner().SetEnabled(a_enabled);
		}
	}
}

//Serialization
RTTR_REGISTRATION
{
	RTTR_REGISTER_CLASS(Helios::Entity, S_ENTITY_NAME, Helios::EntityID, std::size_t, std::string)
	RTTR_PROPERTY(S_ENTITY_PROP_NAME, Helios::Entity::m_name)
	RTTR_PROPERTY_GET_SET(S_ENTITY_PROP_ENABLED, Helios::Entity::IsEnabled, Helios::Entity::SetEnabled)(SET_HIDE_FROM_INSPECTOR)
	RTTR_PROPERTY_READ_ONLY(S_ENTITY_PROP_ENTITY_ID, Helios::Entity::m_entityID)
	RTTR_PROPERTY_READ_ONLY(S_ENTITY_PROP_COMPONENTS, Helios::Entity::GetAllComponents)
	RTTR_PROPERTY(S_ENTITY_PROP_HAS_PREFAB, Helios::Entity::m_hasPrefab)(SET_HIDE_FROM_INSPECTOR)
	RTTR_PROPERTY(S_ENTITY_PROP_PREFAB_PATH, Helios::Entity::m_prefabPath)(SET_HIDE_FROM_INSPECTOR);

	RTTR_REGISTER_CLASS(Helios::EntityID, S_ENTITYID_NAME, std::uint64_t)
	RTTR_PROPERTY(S_ENTITYID_PROP_ID, Helios::EntityID::m_id);
}
