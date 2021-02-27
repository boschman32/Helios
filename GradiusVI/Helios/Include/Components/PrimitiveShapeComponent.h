#pragma once
#include "Components/RenderComponent.h"
#include "Core/Core.h"
#include "Components/MeshRenderer.h"

class Mesh;
class DX12Texture;

namespace Helios
{
	enum class EShapeType
	{
		EShapeType_Plane, EShapeType_Sphere, EShapeType_Box
	};

	class PrimitiveShape : public RenderComponent
	{
        RTTR_ENABLE(Component)
        RTTR_REGISTRATION_FRIEND
	public:
        PrimitiveShape(Entity& a_owner, const ComponentID& a_id);

		void Render() override {}
        const Mesh* GetRenderable() override;

        void SetTexture(const std::shared_ptr<DX12Texture>& a_texture);
        const std::shared_ptr<DX12Texture>& GetTexture() const;

        EShapeType GetCurrentShapeType() const { return m_currentShape; }
        void SetCurrentShapeType(EShapeType a_shapeType);

		EShapeType m_currentShape;
	private:
        std::shared_ptr<DX12Texture> m_pTexture = nullptr;

        MeshAsset m_mesh;

        ResourceArchive GetTextureArchive() const;
        void LoadTextureFromArchive(const ResourceArchive a_archive);
	};
}