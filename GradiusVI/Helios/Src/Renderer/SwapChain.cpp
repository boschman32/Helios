#include "hepch.h"
#include "Renderer/SwapChain.h"

#include "Renderer/DX12Renderer.h"
#include "Renderer/CommandQueue.h"
#include "Renderer/RenderHelpers.h"

#include "Renderer/d3dx12.h"

using namespace Microsoft::WRL;

SwapChain::SwapChain(Window& a_rWindow)
    : m_rWindow(a_rWindow)
{
    DX12Renderer& renderer = DX12Renderer::GetInstance();
    auto device = renderer.GetDevice();

    CreateSwapChain();
    m_RTVDescriptorHeap = renderer.CreateDescriptorHeap(Window::s_bufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_RTVDescriptorSize = renderer.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    UpdateRenderTargetViews();

    // Create the descriptor heap for the depth-stencil view.
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dSVHeap)));

    ResizeDepthBuffer();
}

SwapChain::~SwapChain()
{
    for (int i = 0; i < Window::s_bufferCount; ++i)
    {
        m_backBuffers[i].Reset();
    }
}

void SwapChain::CreateSwapChain()
{
    DX12Renderer& renderer = DX12Renderer::GetInstance();

    ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_rWindow.m_clientWidth;
    swapChainDesc.Height = m_rWindow.m_clientHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc = { 1, 0 };
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = Window::s_bufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    // It is recommended to always allow tearing if tearing support is available.
    swapChainDesc.Flags = m_rWindow.m_isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    ID3D12CommandQueue* pCommandQueue = renderer.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetD3D12CommandQueue().Get();

    ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
        pCommandQueue,
        m_rWindow.m_hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1));

    ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(m_rWindow.m_hWnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain1.As(&m_dxgiSwapChain));

    m_currentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();
}

void SwapChain::UpdateRenderTargetViews()
{
    auto device = DX12Renderer::GetInstance().GetDevice();

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < Window::s_bufferCount; ++i)
    {
        ComPtr<ID3D12Resource> backBuffer;
        ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

        device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

        m_backBuffers[i] = backBuffer;

        rtvHandle.Offset(m_RTVDescriptorSize);
    }
}

void SwapChain::Resize()
{
    for (int i = 0; i < Window::s_bufferCount; ++i)
    {
        m_backBuffers[i].Reset();
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    ThrowIfFailed(m_dxgiSwapChain->GetDesc(&swapChainDesc));
    ThrowIfFailed(m_dxgiSwapChain->ResizeBuffers(Window::s_bufferCount, m_rWindow.m_clientWidth,
        m_rWindow.m_clientHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

    m_currentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

    UpdateRenderTargetViews();

    ResizeDepthBuffer();
}

UINT SwapChain::Present()
{
    UINT syncInterval = m_rWindow.m_vSync ? 1 : 0;
    UINT presentFlags = m_rWindow.m_isTearingSupported && !m_rWindow.m_vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(m_dxgiSwapChain->Present(syncInterval, presentFlags));
    m_currentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

    return m_currentBackBufferIndex;
}

void SwapChain::ResizeDepthBuffer()
{
    DX12Renderer& renderer = DX12Renderer::GetInstance();
    auto device = renderer.GetDevice();

    // Flush any GPU commands that might be referencing the depth buffer.
    renderer.Flush();

    // Resize screen dependent resources.
    // Create a depth buffer.
    D3D12_CLEAR_VALUE optimizedClearValue = {};
    optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    optimizedClearValue.DepthStencil = { 1.0f, 0 };

    CD3DX12_HEAP_PROPERTIES heapPropertiesDefault(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC tex2DDescription = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_rWindow.m_clientWidth, m_rWindow.m_clientHeight,
        1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    ThrowIfFailed(device->CreateCommittedResource(
        &heapPropertiesDefault,
        D3D12_HEAP_FLAG_NONE,
        &tex2DDescription,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optimizedClearValue,
        IID_PPV_ARGS(&m_depthBuffer)
    ));

    // Update the depth-stencil view.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
    dsv.Format = DXGI_FORMAT_D32_FLOAT;
    dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv.Texture2D.MipSlice = 0;
    dsv.Flags = D3D12_DSV_FLAG_NONE;

    device->CreateDepthStencilView(m_depthBuffer.Get(), &dsv,
        m_dSVHeap->GetCPUDescriptorHandleForHeapStart());
}
