#pragma once
#include "Entity.h"

#include <stack>
#include <queue>

namespace sf {
    class RenderWindow;
}

namespace Helios
{
    class Scene;

	class EntityManager
	{
        //Editor stuff for adding entities directly (Or destroying).
        friend class SceneManager;
        friend class SceneGraph;
        friend class EditorHistory;
        friend class PrefabUtility;
        friend class EditorLayer;
		friend class Deserializer;
		friend class Application;
		friend class MenuBar;
	public:
		//Singleton
		static EntityManager& GetInstance()
		{
			static EntityManager instance;

			return instance;
		}
		//Delete the copy constructor and assignment as there should be only one entity manager.
		EntityManager(const EntityManager&) = delete;
		void operator=(const EntityManager&) = delete;

        EntityManager();
        ~EntityManager() = default;

        void Start();

		Entity* CreateEntity();
		Entity* CreateEntity(const std::string& a_name, bool a_addTransform = true);
		
		Entity* GetEntityByIndex(std::size_t a_index);
		Entity* GetEntityByUniqueId(const EntityID& a_id);

		std::vector<Entity*> GetAllEntities() const;
		
		Entity* GetEntityByName(const std::string& a_name);
		void DestroyEntity(Entity* a_entity);
		void Update(float a_deltaTime);
		void Render();

		void DestroyAll();
	private:
		std::vector<std::unique_ptr<Entity>> m_entities;
		std::stack<std::size_t> m_availableSpots;
		std::queue<std::unique_ptr<Entity>> m_entityAddQueue;
		std::set<Entity*> m_entityRemoveQueue;
        std::vector<Entity*> m_allEntities;

		std::size_t m_numIndices;
		std::size_t m_numCreatedEntities;
		std::size_t m_numActiveEntities;

		void AddEntity(std::unique_ptr<Entity> a_entity);
		void RemoveEntity(Entity* a_entity, Scene& a_inScene, bool a_keepListIntact = true);
        void RemoveWithChildren(Entity* a_entity, Scene& a_inScene, bool a_keepListIntact = true);

        //Editor only use
        Entity* CreateEntityDirectly(Scene& a_inScene);
        Entity* CreateEntityDirectly(const std::string& a_name, Scene& a_inScene, bool a_addTransform = true);

        void DestroyEntityDirectly(Entity* a_entity, Scene& a_inScene);
		bool IsWithinView(const Transform& a_entityTrans);
	};
}
