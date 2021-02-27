#pragma once
#include "Resource.h"
#include "Utils/FileUtils.h"

namespace Helios
{
    namespace file_data
    {
        constexpr char* ROOT_FOLDER = ".\\";
        constexpr char* ASSET_FOLDER = "Assets";
        constexpr char* NONE_FILE = "NONE";
    }

	class ResourceManager
	{
        friend class Deserializer;
	public:
		//Singleton
		static ResourceManager& GetInstance()
		{
			static ResourceManager instance;

			return instance;
		}
		ResourceManager() = default;
		~ResourceManager();

		//Delete the copy constructor and assignment as there should be only one resource manager.
		ResourceManager(const ResourceManager&) = delete;
		void operator=(const ResourceManager&) = delete;

		template<typename T, typename... Args, typename = std::enable_if<std::is_base_of<Resource, T>::value, T>>
		std::shared_ptr<T> LoadResource(const std::string& a_path, Args... a_args);

        void UnloadResource(std::shared_ptr<Resource>&& resource);
		void ReleaseAllResources();

		template<typename T, typename = std::enable_if<std::is_base_of<Resource, T>::value, T>>
		std::shared_ptr<T> GetResource(const ResourceId& a_id);

		void Update(float a_deltaTime);
	private:

		std::unordered_map<std::uint64_t, std::shared_ptr<Resource>> m_resources;

        rttr::variant LoadResourceWithArchive(const std::string& a_resourceType, const ResourceArchive& a_archive);
		rttr::variant CreateEmptyResource(ResourceType a_resourceType);
	};

	template <typename T, typename... Args, typename>
	std::shared_ptr<T> ResourceManager::LoadResource(const std::string& a_path, Args... a_args)
	{
		//Hash both the path and name so we can have an unique hash.
		ResourceId idHandle(std::hash<std::string>{}(a_path));
		
		//Check if resource is already loaded in.
		std::shared_ptr<T> resource = GetResource<T>(idHandle);
		if(resource != nullptr)
		{
			return resource;
		}

		resource = std::make_shared<T>(a_args...);
		resource->Create(idHandle, a_path, ExtractFilenameFromPath(a_path));
        resource->OnInitialize();
		m_resources.insert(std::make_pair(idHandle.Value(), resource));

        HE_CORE_INFO("[ResourceManager]: Loaded resource(filename: {0},\npath: {1})", resource->GetFileName(), a_path);

		return resource;
	}

	template <typename T, typename>
	std::shared_ptr<T> ResourceManager::GetResource(const ResourceId& a_id)
	{
		const auto it = m_resources.find(a_id.Value());
		if(it != m_resources.end())
		{
			return std::dynamic_pointer_cast<T>(it->second);
		}
		return nullptr;
	}
}
