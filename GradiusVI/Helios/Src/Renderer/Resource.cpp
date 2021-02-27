#include "hepch.h"
#include "Renderer/Resource.h"

#include "Renderer/DX12Renderer.h"
#include "Renderer/ResourceStateTracker.h"
#include "Renderer/RenderHelpers.h"
#include "Renderer/d3dx12.h"
#include "..\..\Include\Renderer\DX12Texture.h"

DX12Resource::DX12Resource(const std::wstring& name)
    : m_ResourceName(name)
    , m_FormatSupport({})
{}

DX12Resource::DX12Resource(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue, const std::wstring& name)
{
    if (clearValue)
    {
        m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
    }

    auto device = DX12Renderer::GetInstance().GetDevice();

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COMMON,
        m_d3d12ClearValue.get(),
        IID_PPV_ARGS(&m_d3d12Resource)
    ));

    ResourceStateTracker::AddGlobalResourceState(m_d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

    CheckFeatureSupport();
    SetName(name);
}

DX12Resource::DX12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const std::wstring& name)
    : m_d3d12Resource(resource)
    , m_FormatSupport({})
{
    CheckFeatureSupport();
    SetName(name);
}

DX12Resource::DX12Resource(const DX12Resource& copy)
    : m_d3d12Resource(copy.m_d3d12Resource)
    , m_FormatSupport(copy.m_FormatSupport)
    , m_ResourceName(copy.m_ResourceName)
    , m_d3d12ClearValue(std::make_unique<D3D12_CLEAR_VALUE>(*copy.m_d3d12ClearValue))
{}

DX12Resource::DX12Resource(DX12Resource&& copy)
    : m_d3d12Resource(std::move(copy.m_d3d12Resource))
    , m_FormatSupport(copy.m_FormatSupport)
    , m_ResourceName(std::move(copy.m_ResourceName))
    , m_d3d12ClearValue(std::move(copy.m_d3d12ClearValue))
{}

DX12Resource& DX12Resource::operator=(const DX12Resource& other)
{
    if (this != &other)
    {
        m_d3d12Resource = other.m_d3d12Resource;
        m_FormatSupport = other.m_FormatSupport;
        m_ResourceName = other.m_ResourceName;
        if (other.m_d3d12ClearValue)
        {
            m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*other.m_d3d12ClearValue);
        }
    }

    return *this;
}

DX12Resource& DX12Resource::operator=(DX12Resource&& other) noexcept
{
    if (this != &other)
    {
        m_d3d12Resource = std::move(other.m_d3d12Resource);
        m_FormatSupport = other.m_FormatSupport;
        m_ResourceName = std::move(other.m_ResourceName);
        m_d3d12ClearValue = std::move(other.m_d3d12ClearValue);

        other.Reset();
    }

    return *this;
}


DX12Resource::~DX12Resource()
{
}

void DX12Resource::SetD3D12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, const D3D12_CLEAR_VALUE* clearValue)
{
    m_d3d12Resource = d3d12Resource;
    if (m_d3d12ClearValue)
    {
        m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
    }
    else
    {
        m_d3d12ClearValue.reset();
    }
    CheckFeatureSupport();
    SetName(m_ResourceName);
}

void DX12Resource::SetName(const std::wstring& name)
{
    m_ResourceName = name;
    if (m_d3d12Resource && !m_ResourceName.empty())
    {
        m_d3d12Resource->SetName(m_ResourceName.c_str());
    }
}

void DX12Resource::Reset()
{
    m_d3d12Resource.Reset();
    m_FormatSupport = {};
    m_d3d12ClearValue.reset();
    m_ResourceName.clear();
}

bool DX12Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const
{
    return (m_FormatSupport.Support1 & formatSupport) != 0;
}

bool DX12Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const
{
    return (m_FormatSupport.Support2 & formatSupport) != 0;
}

void DX12Resource::CheckFeatureSupport()
{
    if (m_d3d12Resource)
    {
        auto desc = m_d3d12Resource->GetDesc();
        auto device = DX12Renderer::GetInstance().GetDevice();

        m_FormatSupport.Format = desc.Format;
        ThrowIfFailed(device->CheckFeatureSupport(
            D3D12_FEATURE_FORMAT_SUPPORT,
            &m_FormatSupport,
            sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
    }
    else
    {
        m_FormatSupport = {};
    }
}
