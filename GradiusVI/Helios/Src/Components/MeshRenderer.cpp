#include "hepch.h"
#include "Components/MeshRenderer.h"
#include "Core/ResourceManager.h"
#include "Core/Core.h"
namespace Helios
{

    MeshRenderer::MeshRenderer(Entity& a_owner, const ComponentID& a_id) : RenderComponent(a_owner, a_id)
    {
        m_mesh = MeshAsset();
    }

    MeshRenderer::~MeshRenderer()
    {
        if (m_mesh.mesh != nullptr)
        {
            ResourceManager::GetInstance().UnloadResource(std::move(m_mesh.mesh));
            ResourceManager::GetInstance().UnloadResource(std::move(m_pTexture));
            ResourceManager::GetInstance().UnloadResource(std::move(m_pMeshTexture));
        }
    }

    Material* MeshRenderer::GetMaterial()
    {
        return nullptr;
    }

    void MeshRenderer::LoadMeshFromPath(std::string a_filepath)
    {
        m_mesh.m_filepath = a_filepath;
        m_mesh.mesh = ResourceManager::GetInstance().LoadResource<Mesh>(a_filepath, m_material.m_material);
        if (m_pTexture != nullptr)
        {
            m_mesh.mesh->SetTexture(m_pTexture);
        }
        m_defaultMaterial = m_material;
    }

    void MeshRenderer::Render()
    {
    }

    const Mesh* MeshRenderer::GetRenderable()
    {
        if (m_mesh.m_filepath.empty())
        {
            return nullptr;
        }
        else
        {
            if (m_mesh.mesh != nullptr)
            {

                return m_mesh.mesh.get();
            }
            else
            {
                LoadMeshFromPath(m_mesh.m_filepath);
                return nullptr;
            }
        }
    }
    void MeshRenderer::SetTexture(const std::shared_ptr<DX12Texture>& a_pTexture)
    {
        m_pTexture = a_pTexture;

        if (m_mesh.mesh != nullptr)
        {
            m_mesh.mesh->SetTexture(m_pTexture);
        }

    }

    ResourceArchive MeshRenderer::GetTextureArchive() const
    {
        if (m_pTexture != nullptr)
        {
            return m_pTexture->GetResourceArchive();
        }
        return {};
    }
    void MeshRenderer::LoadTextureFromArchive(const ResourceArchive a_archive)
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
            if (m_mesh.mesh != nullptr)
            {
                m_mesh.mesh->SetTexture(m_pTexture);
            }
        }
    }
}
