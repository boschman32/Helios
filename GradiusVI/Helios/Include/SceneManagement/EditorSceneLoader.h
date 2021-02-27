#pragma once

namespace Helios
{
    class Scene;

    class EditorSceneLoader
    {
    public:
        static void SaveSceneAs(Scene& a_scene, const std::string& a_scenePath);
        static void SaveScene(Scene& a_scene);
        static Scene OpenScene(const std::string& a_scenePath);

        static std::string GetSceneNameFromPath(const std::string& a_scenePath);
    };
}
