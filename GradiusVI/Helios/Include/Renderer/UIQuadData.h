#pragma once
#include "Core/Resource.h"
#include "Renderer/DX12Texture.h"
#include "DirectXMath.h"
#include "glm.hpp"


struct UIQuadData
{
public:
    UIQuadData();
    ~UIQuadData();

    void SetTextureResource(const std::shared_ptr<Helios::DX12Texture>& a_texture);
    const std::shared_ptr<Helios::DX12Texture>& GetTextureResource() const;

    const glm::vec2& GetPosition() { return m_position; }
    void SetPosition(const glm::vec2& a_position) { m_position = a_position; }

    const glm::vec2& GetSize() { return m_size; }
    void SetSize(const glm::vec2& a_size) { m_size = a_size; }

    const glm::vec2& GetAnchorPoint() { return m_anchorPoint; }
    void SetAnchorPoint(const glm::vec2& a_anchorPoint) { m_anchorPoint = a_anchorPoint; }

    float GetRotation() { return m_rotation; }
    void SetRotation(float a_rotation) { m_rotation = a_rotation; }

    DirectX::XMMATRIX GetWorldMatrix(float a_screenWidth, float a_screenHeight) const;


private:
    std::shared_ptr<Helios::DX12Texture> m_primitiveTextureResource = nullptr;

    //(0,0) is top left, (1,1) is bottom right
    glm::vec2 m_position = glm::vec2(0.f);

    //(1,1) for a quad as big as the screen
    glm::vec2 m_size = glm::vec2(0.f);

    //(-1,-1) draws quad from top left, (1,1) from bottom right
    glm::vec2 m_anchorPoint = glm::vec2(0.f);

    float m_rotation = 0.0f;
};

