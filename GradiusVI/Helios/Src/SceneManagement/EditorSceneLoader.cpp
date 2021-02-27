#include "hepch.h"
#include "SceneManagement/EditorSceneLoader.h"
#include "SceneManagement/SceneManager.h"

#include "Utils/FileUtils.h"
#include "Utils/Serializer.h"

namespace Helios
{
    void EditorSceneLoader::SaveSceneAs(Scene& a_scene, const std::string& a_scenePath)
    {
        HE_CORE_INFO("[SceneManagement] Saving scene...");
        a_scene.RenameScene(GetSceneNameFromPath(a_scenePath));
        a_scene.SetScenePath(a_scenePath);
        const std::string jsonMeta = Serializer::Serialize(a_scene);

        //Save the .scene asset file.
        SaveFile(a_scenePath, jsonMeta);
        HE_CORE_INFO("[SceneManagement] Scene saved...");
    	a_scene.SetValid(true);
    }

    void EditorSceneLoader::SaveScene(Scene& a_scene)
    {
        HE_CORE_INFO("[SceneManagement] Saving scene...");
        const std::string jsonMeta = Serializer::Serialize(a_scene);
        SaveFile(a_scene.GetScenePath(), jsonMeta);
        HE_CORE_INFO("[SceneManagement] Scene saved...");
    }

    Scene EditorSceneLoader::OpenScene(const std::string& a_scenePath)
    {
        HE_CORE_INFO("[SceneManagement] Opening scene...");
        Scene sceneToOpen;
        std::string jsonMeta;
        if(ReadFromFileToString(a_scenePath, jsonMeta))
        {
        	Deserializer::DeserializeScene(sceneToOpen, jsonMeta);
            SceneManager::GetInstance().SetActiveScene(sceneToOpen);
        }
        else
        {
            HE_CORE_ERROR("Couldn't open scene on path: {0}", a_scenePath);
            sceneToOpen.SetValid(false);
        }

        HE_CORE_INFO("[SceneManagement] Scene opened...");
        return sceneToOpen;
    }

    std::string EditorSceneLoader::GetSceneNameFromPath(const std::string& a_scenePath)
    {
        const std::string fullName = a_scenePath.substr(a_scenePath.find_last_of('\\') + 1);
        const std::string sceneName = fullName.substr(0, fullName.find('.'));
        return sceneName;
    }
}
