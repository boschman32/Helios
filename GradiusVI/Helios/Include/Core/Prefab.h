#pragma once
#include "Core.h"
#include "Core/Resource.h"
#include "Components/Transform.h"

namespace Helios
{
    constexpr char* PREFAB_EXTENSION = ".prefab";
    constexpr char* DEFAULT_PREFAB_PATH = ".\\Assets\\Prefabs\\";

	class Entity;
	class Prefab : public Resource
	{
        friend class PrefabUtility;
		RTTR_ENABLE(Resource)
	public:
		Prefab();
		Entity* Spawn(const Vec3& a_spawnPos = Vec3(0.f), bool a_useSerializedPos = false) const;
		
		void OnInitialize() override;
		void Reload() override;
	private:
		std::string m_prefabData;
	};

    class PrefabUtility
    {
    public:
        static Prefab* LoadPrefab(const std::string& a_path);
        static void UpdatePrefabResource(const std::string& a_prefabPath);
		static void UpdateClones(const EntityID& updateCloneId, const std::string& a_prefabPath, const std::string& a_prefabMeta);
        static void UnloadPrefab(const std::string& a_path);

        static void UnloadAll();

         //As we want to unload a prefab when nobody is using this prefab anymore (since they are resources)
	    //we need something to keep track of how many prefabs there are used per prefab.
        static std::unordered_map<std::string, std::pair<std::uint32_t, std::shared_ptr<Prefab>>> m_prefabUseCount;
    };
}

