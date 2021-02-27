#pragma once

namespace Helios
{
	class Entity;
	
	class Scene
	{
		RTTR_ENABLE()
	public:
		Scene(std::string a_sceneName = "Untitled Scene");

		void RenameScene(const std::string& a_sceneName) { m_sceneName = a_sceneName; }
		
		const std::string& GetSceneName() const { return m_sceneName; }
		const std::string& GetScenePath() const { return m_scenePath; }
		void SetScenePath(const std::string& a_scenePath) { m_scenePath = a_scenePath; }
		
		bool IsLoaded() const { return m_isLoaded; }
		void SetLoaded(bool a_loaded) { m_isLoaded = a_loaded; }

        bool IsValid() const { return m_isValid; }
        void SetValid(bool a_valid) {  m_isValid = a_valid; }
		
		void AddEntity(Entity* a_entity);
		void RemoveEntity(Entity* a_entity);

		void SetSelectedEntity(Entity* a_entity);
		Entity* GetSelectedEntity() const;
		
        std::vector<Entity*>& GetEntitiesInScene() { return m_entitiesInScene; };
	private:
		std::string m_sceneName;
		std::string m_scenePath;
		Entity* m_selectedEntity = nullptr;
		bool m_isLoaded;
        bool m_isValid;
		
		std::vector<Entity *> m_entitiesInScene;

		RTTR_REGISTRATION_FRIEND
	};
}

