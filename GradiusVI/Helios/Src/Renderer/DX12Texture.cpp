#include "hepch.h"
#include "Renderer/DX12Texture.h"

#include <Renderer/DX12Renderer.h>
#include <Renderer/CommandQueue.h>
#include <Renderer/CommandList.h>

namespace Helios
{
    DX12Texture::DX12Texture() :
        Resource(ResourceType::TEXTURE)
    {

    }

    Helios::DX12Texture::~DX12Texture()
    {
    }

    void DX12Texture::OnInitialize()
    {
        auto device = DX12Renderer::GetInstance().GetDevice();
        auto commandQueue = DX12Renderer::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
        auto commandList = commandQueue->GetCommandList();

        std::string pathAsString = m_path;
        std::wstring pathAsWString(pathAsString.length(), L' '); // Make room for characters
        std::copy(pathAsString.begin(), pathAsString.end(), pathAsWString.begin());

        commandList->LoadTextureFromFile(m_texture, pathAsWString);

        auto fenceValue = commandQueue->ExecuteCommandList(commandList);
        commandQueue->WaitForFenceValue(fenceValue);
    }

    void DX12Texture::Reload() 
    {

    }
}