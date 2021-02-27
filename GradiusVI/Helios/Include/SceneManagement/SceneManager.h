#pragma once
#include "Scene.h"

namespace Helios
{
    class EntityManager;

	constexpr char* SCENE_EXTENSION_NAME = ".scene";
	constexpr char* SCENE_ASSET_PATH = "./Assets/Scenes/";
	
	class SceneManager
	{
	public:
        //Singleton
        static SceneManager& GetInstance()
        {
            static SceneManager instance;

            return instance;
        }
        //Delete the copy constructor and assignment as there should be only one resource manager.
        SceneManager(const SceneManager&) = delete;
        void operator=(const SceneManager&) = delete;

		SceneManager() = default;

		Scene& GetActiveScene() { return m_activeScene;  }
        void SetActiveScene(Scene& a_scene);

		Scene& CreateScene();
        Scene& CreateScene(const std::string& a_sceneName);

		Scene& LoadScene(const std::string& a_scenePath);
	private:
		Scene m_activeScene;
	};
}
