#include "hepch.h"

#include "Components/PrimitiveShapeComponent.h"
#include "Core/ResourceManager.h"

namespace Helios
{
    PrimitiveShape::PrimitiveShape(Entity& a_owner, const ComponentID& a_id)
        : RenderComponent(a_owner, a_id), m_currentShape(EShapeType::EShapeType_Plane)
    {
        switch (m_currentShape)
        {
        case EShapeType::EShapeType_Plane:
            m_mesh.mesh = Mesh::CreatePlane();
            break;
        case EShapeType::EShapeType_Sphere:
            m_mesh.mesh = Mesh::CreateSphere();
            break;
        case EShapeType::EShapeType_Box:
            m_mesh.mesh = Mesh::CreateCube();
            break;
        default:
            break;
        }
    }

    const Mesh* PrimitiveShape::GetRenderable()
    {
        return m_mesh.mesh.get();
    }

    void PrimitiveShape::SetTexture(const std::shared_ptr<DX12Texture>& a_pTexture)
    {
        m_pTexture = a_pTexture;

        if (m_mesh.mesh != nullptr)
        {
            m_mesh.mesh->SetTexture(a_pTexture);
        }
    }

    const std::shared_ptr<DX12Texture>& PrimitiveShape::GetTexture() const
    {
        return m_pTexture;
    }

    ResourceArchive PrimitiveShape::GetTextureArchive() const
    {
        if (m_pTexture != nullptr)
        {
            return m_pTexture->GetResourceArchive();
        }
        return {};
    }

    void PrimitiveShape::LoadTextureFromArchive(const ResourceArchive a_archive)
    {
        //Unload the texture if the new archive is not the same texture.
        if (m_pTexture != nullptr && m_pTexture->GetFileName() != a_archive.m_fileName && m_pTexture->GetPath() != a_archive.m_path)
        {
            ResourceManager::GetInstance().UnloadResource(std::move(m_pTexture));
            m_pTexture = nullptr;
        }

        //Don't start loading a archive that has no path
        if (a_archive.m_path.empty())
        {
            return;
        }

        //Load a new texture.
        if (m_pTexture == nullptr)
        {
            m_pTexture = ResourceManager::GetInstance().LoadResource<Helios::DX12Texture>(a_archive.m_path);
        	if(m_mesh.mesh != nullptr)
        	{
        		m_mesh.mesh->SetTexture(m_pTexture);
        	}
        }
    }

    void PrimitiveShape::SetCurrentShapeType(EShapeType a_shapeType)
    {
        m_currentShape = a_shapeType;
        switch (m_currentShape)
        {
        case EShapeType::EShapeType_Plane:
            m_mesh.mesh = Mesh::CreatePlane();
            break;
        case EShapeType::EShapeType_Sphere:
            m_mesh.mesh = Mesh::CreateSphere();
            break;
        case EShapeType::EShapeType_Box:
            m_mesh.mesh = Mesh::CreateCube();
            break;
        default:;
        }
    };
}
