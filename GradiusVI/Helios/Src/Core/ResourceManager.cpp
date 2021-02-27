#include "hepch.h"
#include "Core/ResourceManager.h"
#include "Core/Core.h"
#include "Core/Prefab.h"
#include "Renderer/DX12Texture.h"

namespace Helios
{
    ResourceManager::~ResourceManager()
    {
        //Check if all the resources are only being used by one instance (the resource manager).
        for (const auto& r : m_resources)
        {
            if (r.second.use_count() > 1)
            {
                printf("Resource still in use: %s\n", r.second->GetFileName().c_str());
                assert(false && "Resource still in use!");
            }
            else
            {
                 printf("[ResourceManager]: Unloaded resource(filename: %s, type: %s)\n", 
                     r.second->m_fileName.c_str(), rttr::type::get(r.second).get_name().to_string().c_str());
            }
        }
    }

    void ResourceManager::UnloadResource(std::shared_ptr<Resource>&& resource)
    {
        if (resource != nullptr)
        {
            if (resource.use_count() <= 2)
            {
                const auto found = m_resources.find(resource->GetID().Value());
                if (found != m_resources.end())
                {
                    HE_CORE_INFO("[ResourceManager]: Unloaded resource(filename:{0}, type:{1})", resource->m_fileName, rttr::type::get(resource).get_name());
                    m_resources.erase(found);
                }
            }
        }
    }

    rttr::variant ResourceManager::LoadResourceWithArchive(const std::string& a_resourceType, const ResourceArchive& a_archive)
    {
        const rttr::type classType = rttr::type::get_by_name(a_resourceType);
        //Hash both the path and name so we can have an unique hash.
        const ResourceId idHandle(std::hash<std::string>{}(a_archive.m_path));

        //Check if resource is already loaded in.
        const auto& found = m_resources.find(idHandle.Value());
        if (found != m_resources.end())
        {
            rttr::variant r = found->second;
            //Convert the resource base to the child.
            if (r.convert(rttr::type::get_by_name("classstd::shared_ptr<classHelios::" + a_resourceType + ">")))
            {
                return r;
            }
            return nullptr;
        }

        rttr::variant createdResource = classType.create();

        std::shared_ptr<Resource> resource = createdResource.get_value<std::shared_ptr<Resource>>();
        resource->Create(idHandle, a_archive.m_path, a_archive.m_fileName);
        resource->OnInitialize();
        m_resources.insert(std::make_pair(idHandle.Value(), resource));

        HE_CORE_INFO("[ResourceManager]: Loaded resource(filename: {0},\npath: {1})", resource->GetFileName(), resource->GetPath());

        return createdResource;
    }

    rttr::variant ResourceManager::CreateEmptyResource(ResourceType a_resourceType)
    {
        switch (a_resourceType)
        {
        case ResourceType::TEXTURE:
            return std::shared_ptr<DX12Texture>();
        case ResourceType::PREFAB:
            return std::shared_ptr<Prefab>();
        case ResourceType::NONE:;
        default:
            HE_CORE_WARN("Cannot create empty resource of type: {0} as it is not supported!", a_resourceType);
            return nullptr;
        }
    }

    void ResourceManager::ReleaseAllResources()
    {
        m_resources.clear();
    }

    void ResourceManager::Update(float)
    {
        HE_CORE_ASSERT(false, "Not yet implemented!");
    }
}
