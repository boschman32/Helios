#include "hepch.h"
#include "Renderer/Buffer.h"

Buffer::Buffer(const std::wstring& name)
    : DX12Resource(name)
{}

Buffer::Buffer(const D3D12_RESOURCE_DESC& resDesc,
    size_t numElements, size_t elementSize,
    const std::wstring& name)
    : DX12Resource(resDesc, nullptr, name)
{
    numElements;
    elementSize;
}
