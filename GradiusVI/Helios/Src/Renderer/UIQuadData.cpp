#include "hepch.h"
#include "Renderer/UIQuadData.h"
#include "Renderer/RenderHelpers.h"

using namespace DirectX;

UIQuadData::UIQuadData()
{
}

UIQuadData::~UIQuadData()
{
}

void UIQuadData::SetTextureResource(const std::shared_ptr<Helios::DX12Texture>& a_texture)
{
	m_primitiveTextureResource = a_texture;
}

//void UIQuadData::SetTexture(const std::shared_ptr<Helios::DX12Texture>& a_texture)
//{
//    a_texture;
//}

const std::shared_ptr<Helios::DX12Texture>& UIQuadData::GetTextureResource() const
{
    return m_primitiveTextureResource;
}

DirectX::XMMATRIX UIQuadData::GetWorldMatrix(float a_screenWidth, float a_screenHeight) const
{
    glm::vec2 textureSize(0.f, 0.f);
    if (m_primitiveTextureResource != nullptr)
    {
        const Texture& texture = m_primitiveTextureResource->GetInternalTexture();
        textureSize.x = static_cast<float>(texture.m_width);
        textureSize.y = static_cast<float>(texture.m_height);
    }

    glm::vec2 size = textureSize * m_size;

    glm::vec2 position(
        a_screenWidth * m_position.x - (m_anchorPoint.x * (size.x / 2.0f)),
            a_screenHeight * m_position.y - (m_anchorPoint.y * (size.y / 2.0f))
            );

    XMMATRIX translationMatrix = XMMatrixTranslation(position.x, -position.y, 600.0f);
    XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) * XMMatrixRotationZ(XMConvertToRadians(m_rotation));
    XMMATRIX scaleMatrix = XMMatrixScaling(size.x, 100.f, size.y);

    DirectX::XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
    return worldMatrix;
}
