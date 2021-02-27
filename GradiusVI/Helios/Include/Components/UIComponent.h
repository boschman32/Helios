#pragma once
#include "Components/RenderComponent.h"
#include "Renderer/UIQuadData.h"
namespace Helios
{
	class UIComponent : public RenderComponent
	{
	RTTR_ENABLE(Component)
	RTTR_REGISTRATION_FRIEND
	public:
		UIComponent(Entity& a_owner, const ComponentID& a_id);

		void Render() override;
		const UIQuadData* Get2DRenderable() override;
		void SetTexture(const std::shared_ptr<DX12Texture>& a_pTexture);
		const std::shared_ptr<DX12Texture>& UIComponent::GetTexture() const { return m_texture; };

		void SetTextureFile(const std::string& a_filepath);

		glm::vec2 m_position;
		glm::vec2 m_anchorpoint;
		glm::vec2 m_scale;
		float m_rotation = 0;
		
	private:
		ResourceArchive GetTextureArchive() const;
		void LoadTextureFromArchive(const ResourceArchive a_archive);
		
		UIQuadData m_quadData;
		std::shared_ptr<Helios::DX12Texture> m_texture = nullptr;
	};
}