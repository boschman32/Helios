#include "hepch.h"
#include "SceneManagement/SceneManager.h"

#include "Core/Core.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Core/Application.h"

#include "Utils/FileUtils.h"
#include "Utils/Serializer.h"

#include "Renderer/Window.h"

namespace Helios
{
	Scene& SceneManager::CreateScene()
	{
		m_activeScene = Scene();
		m_activeScene.SetValid(false);
		return m_activeScene;
	}

	Scene& SceneManager::CreateScene(const std::string& a_sceneName)
	{
		m_activeScene = Scene(a_sceneName);
		m_activeScene.SetValid(false);
		return m_activeScene;
	}

	void SceneManager::SetActiveScene(Scene& a_scene)
	{
		if (!a_scene.IsLoaded())
		{
			HE_CORE_WARN("[Scene Management] Setting scene: {0} as active while not loaded in (Use LoadScene).", a_scene.GetSceneName());
			return;
		}
		m_activeScene = a_scene;
	}

	Scene& SceneManager::LoadScene(const std::string& a_scenePath)
	{
		if (m_activeScene.GetScenePath() == a_scenePath)
		{
			return m_activeScene;
		}

		std::string jsonMeta;
		if (ReadFromFileToString(a_scenePath, jsonMeta))
		{
			Scene loadedScene;
			Deserializer::DeserializeScene(loadedScene, jsonMeta);
			SetActiveScene(loadedScene);
		}
		else
		{
			HE_CORE_ASSERT(false, "Couldn't open scene!");
		}

		return m_activeScene;
	}
}
