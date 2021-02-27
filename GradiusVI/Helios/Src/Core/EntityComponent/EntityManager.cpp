#include "hepch.h"
#include "Core/EntityComponent/EntityManager.h"

#include "SceneManagement/SceneManager.h"
#include "Components/Transform.h"
#include "Core/ResourceManager.h"

#include "Core/Prefab.h"
#include "Editor/EditorHistory.h"
#include "Utils/Serializer.h"

namespace Helios
{
    EntityManager::EntityManager()
        : m_numIndices(0), m_numCreatedEntities(0), m_numActiveEntities(0)
    {

    }

    void EntityManager::Start()
    {
        for (auto& e : m_entities)
        {
            for (auto s : e->GetScriptableComponents())
            {
                s->Start();
            }
        }
    }

    Entity* EntityManager::CreateEntity()
    {
        return CreateEntity("Entity_" + std::to_string(m_availableSpots.empty() ? m_numIndices : m_availableSpots.top()));
    }

    Entity* EntityManager::CreateEntity(const std::string& a_name, bool a_addTransform)
    {
        std::size_t availableSpot;
        if (!m_availableSpots.empty())
        {
            availableSpot = m_availableSpots.top();
            m_availableSpots.pop();
        }
        else
        {
            availableSpot = m_numIndices;
            m_numIndices++;
        }

        EntityID uniqueId(m_numCreatedEntities++);
        m_entityAddQueue.push(std::make_unique<Entity>(uniqueId, availableSpot, a_name));

        if (a_addTransform)
        {
            m_entityAddQueue.back()->AddComponent<Transform>();
        }

        m_allEntities.push_back(m_entityAddQueue.back().get());
        return m_entityAddQueue.back().get();
    }

    Entity* EntityManager::GetEntityByIndex(std::size_t a_index)
    {
        if (a_index >= 0 && a_index < m_entities.size())
        {
            return m_entities[a_index].get();
        }
        return nullptr;
    }

    Entity* EntityManager::GetEntityByUniqueId(const EntityID& a_id)
    {
        for (auto& it : m_entities)
        {
            if (it != nullptr && it->GetID() == a_id)
            {
                return it.get();
            }
        }

        return nullptr;
    }

    std::vector<Entity*> EntityManager::GetAllEntities() const
    {
        std::vector<Entity*> entities;
        for (auto& e : m_entities)
        {
            if (e != nullptr)
            {
                entities.push_back(e.get());
            }
        }
        return entities;
    }

    Entity* EntityManager::GetEntityByName(const std::string& a_name)
    {
        const auto it = std::find_if(m_allEntities.begin(), m_allEntities.end(), [a_name](const auto e)
        {
            return e->GetName() == a_name;
        });

        if(it != m_allEntities.end())
        {
            return (*it);
        }

        return nullptr;
    }

    void EntityManager::DestroyEntity(Entity* a_entity)
    {
        if (a_entity != nullptr)
        {
            m_entityRemoveQueue.insert(a_entity);
        }
    }

    void EntityManager::AddEntity(std::unique_ptr<Entity> a_entity)
    {
        //We are only "loaded" if we are enabled otherwise we will be loaded when the entity is set to enabled again.
        if (a_entity->IsEnabled())
        {
            a_entity->m_isLoaded = true;

            //For any scriptable components on this entity we are now starting.
            std::vector<ScriptableComponent*>& scripts = a_entity->GetScriptableComponents();
            if (!scripts.empty())
            {
                for (auto s : scripts)
                {
                    s->Start();
                }
            }
        }

        //Insert the entity on the spot of it's ID.
        SceneManager::GetInstance().GetActiveScene().AddEntity(a_entity.get());

        const std::size_t index = a_entity->GetIndex();
        //We can place the entity on the spot of its index if the index is in the vectors range.
        if (index < m_entities.size())
        {
            m_entities[index] = std::move(a_entity);
        }
        else
        {
            m_entities.push_back(std::move(a_entity));
        }
        m_numActiveEntities++;
    }

    void EntityManager::RemoveEntity(Entity* a_entity, Scene& a_inScene, bool a_keepListIntact)
    {
		EditorHistory::AddChange(EditorHistory::EditorChange::EChangeType::CHANGE_TYPE_ENTITY, Serializer::Serialize(*a_entity), "");
        //Remove ourselves from the parent if we have one.
        if (a_entity->GetTransform().GetParent() != nullptr)
        {
            a_entity->GetTransform().GetParent()->RemoveChild(a_entity->GetTransform());
        }

        for (auto c : a_entity->GetAllComponents())
        {
            a_entity->RemoveComponent(c);
        }
        a_inScene.RemoveEntity(a_entity);
        if (a_entity == a_inScene.GetSelectedEntity())
        {
            a_inScene.SetSelectedEntity(nullptr);
        }

        const auto it = std::find_if(m_entities.begin(), m_entities.end(), [a_entity](const auto& e)
            {
                return e.get() == a_entity;
            });

        if (it != m_entities.end())
        {
            if (a_keepListIntact)
            {
                const std::size_t index = a_entity->GetIndex();
                //Free the memory of this entity but still keep track of it so that another entity may take it's spot.
                m_availableSpots.push(index);
                m_entities[index].reset();
                m_numActiveEntities--;
            }
            else
            {
                m_numIndices--;
                m_numActiveEntities--;
                m_entities.erase(it);
            }

            const auto itAll = std::find_if(m_allEntities.begin(), m_allEntities.end(), [a_entity](const auto e)
                {
                    return e == a_entity;
                });

            if(itAll != m_allEntities.end())
            {
                m_allEntities.erase(itAll);
            }
        }
    }

    void EntityManager::RemoveWithChildren(Entity* a_entity, Scene& a_inScene, bool a_keepListIntact)
    {
        if (a_entity->GetTransform().GetChildrenCount() > 0)
        {
            //Make a copy of the children since we are going to alter it.
            std::vector<Transform*> children = a_entity->GetTransform().GetChildren();
            for (const auto child : children)
            {
                RemoveWithChildren(&child->GetOwner(), a_inScene, a_keepListIntact);
            }
            RemoveEntity(a_entity, a_inScene, a_keepListIntact);
        }
        else
        {
            RemoveEntity(a_entity, a_inScene, a_keepListIntact);
        }
    }

    void EntityManager::Update(float a_deltaTime)
    {
        OPTICK_CATEGORY(OPTICK_FUNC, Optick::Category::GameLogic);

        while (!m_entityAddQueue.empty())
        {
            AddEntity(std::move(m_entityAddQueue.front()));
            m_entityAddQueue.pop();
        }

        //Update entities and their components
        for (auto& entity : m_entities)
        {
            if (entity != nullptr && entity->IsEnabled())
            {
                entity->Update(a_deltaTime);
            }
        }

        // Flush add queue again.
        while (!m_entityAddQueue.empty())
        {
            AddEntity(std::move(m_entityAddQueue.front()));
            m_entityAddQueue.pop();
        }

        while (!m_entityRemoveQueue.empty())
        {
            auto itr = std::prev(m_entityRemoveQueue.end());

            Entity* e = *itr;

            if (e->GetTransform().GetChildrenCount() > 0)
            {
                RemoveWithChildren(e, SceneManager::GetInstance().GetActiveScene());
            }
            else
            {
                RemoveEntity(e, SceneManager::GetInstance().GetActiveScene());
            }
            m_entityRemoveQueue.erase(itr);

            //If no more entities are present we can just clear the list and start anew.
            if (m_numActiveEntities == 0)
            {
                m_entities.clear();
                m_numIndices = 0;
                m_numCreatedEntities = 0;
            }
        }
    }

    void EntityManager::Render()
    {
        OPTICK_CATEGORY(OPTICK_FUNC, Optick::Category::Rendering);
        for (auto& entity : m_entities)
        {
            if (entity != nullptr)
            {
                if (entity->IsEnabled())
                {
                    for (auto renderer : entity->GetRenderComponents())
                    {
                        if (!renderer->IsEnabled())
                        {
                            continue;
                        }

                        if (IsWithinView(entity->GetTransform()))
                        {
                            renderer->Render();
                        }
                    }
                }
            }
        }
    }

    bool EntityManager::IsWithinView(const Transform& a_entityTrans)
    {
        return a_entityTrans.GetOwner().GetTransform().GetPosition().x > 0 &&
            a_entityTrans.GetOwner().GetTransform().GetPosition().y > 0 &&
            a_entityTrans.GetOwner().GetTransform().GetPosition().x < 2000 &&
            a_entityTrans.GetOwner().GetTransform().GetPosition().y < 2000;
    }


    void EntityManager::DestroyAll()
    {
        m_availableSpots = std::stack<std::size_t>();
        m_entityAddQueue = std::queue<std::unique_ptr<Entity>>();
        m_entityRemoveQueue.clear();
        m_allEntities.clear();
        m_entities.clear();
        m_numIndices = 0;
        m_numCreatedEntities = 0;
        m_numActiveEntities = 0;
    }

    //Used for editor stuff (since we don't want to add it through the queue).
    Entity* EntityManager::CreateEntityDirectly(Scene& a_inScene)
    {
        return CreateEntityDirectly("Entity_" + std::to_string(m_numIndices), a_inScene);
    }

    Entity* EntityManager::CreateEntityDirectly(const std::string& a_name, Scene& a_inScene, bool a_addTransform)
    {
        EntityID id(m_numCreatedEntities++);
        m_entities.push_back(std::make_unique<Entity>(id, m_numIndices++, a_name));
        m_numActiveEntities++;

        Entity* addedEntity = m_entities.back().get();
        if (a_addTransform)
        {
            addedEntity->AddComponent<Transform>();
        }

        a_inScene.AddEntity(addedEntity);
        return addedEntity;
    }

    void EntityManager::DestroyEntityDirectly(Entity* a_entity, Scene& a_inScene)
    {
        if (a_entity != nullptr)
        {

            if (a_entity->GetTransform().GetChildrenCount() > 0)
            {
                RemoveWithChildren(a_entity, a_inScene, false);
            }
            else
            {
                RemoveEntity(a_entity, a_inScene, false);
            }
        }
    }

}
