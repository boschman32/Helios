#pragma once
#include "Core/Core.h"

namespace Helios
{
    enum class ResourceType
    {
        NONE,
        TEXTURE,
        MESH,
        AUDIO,
        FONT,
        PREFAB
    };

    //Resource archives are used to store only data for serialization.
    struct ResourceArchive
    {
        RTTR_ENABLE();
    public:
        ResourceArchive(ResourceType a_type = ResourceType::NONE,
            const std::string& a_path = "",
            const std::string& a_fileName = "")
            : m_type(a_type), m_path(a_path), m_fileName(a_fileName) { }
        ResourceType m_type         { ResourceType::NONE };
        std::string m_path          { "" };
        std::string m_fileName      { "" };
    };

    class Resource
    {
        RTTR_ENABLE();
        friend class ResourceManager;
    public:
        virtual ~Resource() = default;

        const ResourceId& GetID() const { return m_id; }
        const std::string& GetPath() const { return m_path; }
        const std::string& GetFileName() const { return m_fileName; }
        ResourceType GetType() const { return m_type; }
        bool IsLoaded() const { return m_isLoaded; }

        //Used for serialization and deserialization.
        ResourceArchive GetResourceArchive() const
        {
            return
            {
                m_type,
                m_path,
                m_fileName,
            };
        };

        bool m_isReloaded = false;

         virtual void Reload() { };
    protected:
        Resource(ResourceType a_type)
            : m_type(a_type),
            m_path(""),
            m_fileName(""),
            m_id(0),
            m_isLoaded(false) { }

        virtual void OnInitialize() { };

        ResourceType m_type;
        std::string m_path;
        std::string m_fileName;
    private:
        //Resource should only be created by the resource manager.
        void Create(ResourceId a_id, const std::string& a_path, const std::string& a_fileName)
        {
            m_id = a_id;
            m_path = a_path;
            m_fileName = a_fileName;
            m_isLoaded = true;
        }

        ResourceId m_id;
        bool m_isLoaded;

        RTTR_REGISTRATION_FRIEND;
    };
}