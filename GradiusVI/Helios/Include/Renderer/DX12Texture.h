#pragma once
#include "Core/Resource.h"

#include "Renderer/Texture.h"

namespace Helios
{
    class DX12Texture : public Helios::Resource
    {
        RTTR_ENABLE()
    public:
        DX12Texture();
        ~DX12Texture() override;

        void OnInitialize() override;
        void Reload() override;

        Texture& GetInternalTexture() { return m_texture; }

    private:
        Texture m_texture;

        RTTR_REGISTRATION_FRIEND
    };
}

