#include "hepch.h"
#include "Components/UIComponent.h"
#include "Core/ResourceManager.h"

namespace Helios
{
	UIComponent::UIComponent(Entity& a_owner, const ComponentID& a_id): RenderComponent(a_owner, a_id), m_scale(glm::vec2(1,1))
	{
	}
	
	void UIComponent::Render()
	{
		
	}

	const UIQuadData* UIComponent::Get2DRenderable()
	{
		if(m_texture != nullptr)
		{
			m_quadData.SetTextureResource(m_texture);
		}
		
		m_quadData.SetPosition(m_position);
		m_quadData.SetRotation(m_rotation);
		m_quadData.SetAnchorPoint(m_anchorpoint);
		m_quadData.SetSize(m_scale);
		
		return &m_quadData;
	}

	void UIComponent::SetTexture(const std::shared_ptr<DX12Texture>& a_pTexture)
	{
		if(a_pTexture!=nullptr)
		{
			m_texture = a_pTexture;
		}
	}

	void UIComponent::SetTextureFile(const std::string& a_filepath)
	{
		if (m_texture != nullptr && m_texture->GetPath() != a_filepath)
		{
			ResourceManager::GetInstance().UnloadResource(std::move(m_texture));
			m_texture = nullptr;
		}

        if(m_texture == nullptr)
        {
            m_texture = ResourceManager::GetInstance().LoadResource<DX12Texture>(a_filepath);
        }
	}

	ResourceArchive UIComponent::GetTextureArchive() const
	{
		if(m_texture!=nullptr)
		{
			return m_texture->GetResourceArchive();
		}
		return{};
	}

	void UIComponent::LoadTextureFromArchive(const ResourceArchive a_archive)
	{
		//Unload the texture if the new archive is not the same texture.
		if (m_texture != nullptr && m_texture->GetFileName() != a_archive.m_fileName && m_texture->GetPath() != a_archive.m_path)
		{
			ResourceManager::GetInstance().UnloadResource(std::move(m_texture));
			m_texture = nullptr;
		}

		//Don't start loading a archive that has no path
		if (a_archive.m_path.empty())
		{
			return;
		}

		//Load a new texture.
		if (m_texture == nullptr)
		{
			//m_mesh.mesh->SetTexture(m_pTexture);
			 m_texture = ResourceManager::GetInstance().LoadResource<DX12Texture>(a_archive.m_path);
		}
	}
}
