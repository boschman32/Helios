#pragma once
#include "Components/RenderComponent.h"
#include "Renderer/Material.h"
#include "Core/Resource.h"
#include "Renderer/DX12Texture.h"

namespace Helios
{
	class MeshAsset 
	{
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	public:
		std::shared_ptr<Mesh> mesh = nullptr;
		std::string m_filepath;
	};

	struct MaterialAsset
	{
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	public:
		std::string m_filepath;
		Material m_material;
	};

	class MeshRenderer : public RenderComponent
	{
		RTTR_ENABLE(Component)
		RTTR_REGISTRATION_FRIEND
	public:
		MeshRenderer(Entity& a_owner, const ComponentID& a_id);
        ~MeshRenderer();
        void Render() override;
		Material* GetMaterial();
		void LoadMeshFromPath(std::string a_filepath);
		const Mesh* GetRenderable() override;

        void SetTexture(const std::shared_ptr<DX12Texture>& a_texture);
        const std::shared_ptr<DX12Texture>& GetTexture() const  { return m_pTexture; }

        ResourceArchive GetTextureArchive() const;
        void LoadTextureFromArchive(const ResourceArchive a_archive);

	private:
        std::shared_ptr<DX12Texture> m_pTexture = nullptr;
        std::shared_ptr<DX12Texture> m_pMeshTexture = nullptr;

		MeshAsset m_mesh;
		MaterialAsset m_material;
        MaterialAsset m_defaultMaterial;
	};


}

