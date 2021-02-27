#include "hepch.h"
#include "Renderer/Window.h"

#include "Renderer/DX12Renderer.h"
#include "Renderer/CommandQueue.h"
#include "Renderer/CommandList.h"
#include "Renderer/SwapChain.h"
#include "Renderer/RenderHelpers.h"
#include "Renderer/ResourceStateTracker.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Renderer/d3dx12.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <algorithm>
#include <cassert>
#include "Core/Core.h"

Window::~Window()
{
    if (DX12Renderer::GetIsInitialized())
    {
        DX12Renderer::GetInstance().Flush();
    }

    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }
}

void Window::Resize(uint32_t a_width, uint32_t a_height)
{
    if (m_clientWidth != a_width || m_clientHeight != a_height)
    {
        m_clientWidth = std::max(static_cast<uint32_t>(1), a_width);
        m_clientHeight = std::max(static_cast<uint32_t>(1), a_height);

        DX12Renderer::GetInstance().Flush();

        // Release all references to back buffer textures.
        m_renderTarget.AttachTexture(Color0, Texture());
        for (int i = 0; i < s_bufferCount; ++i)
        {
            ResourceStateTracker::RemoveGlobalResourceState(m_backBufferTextures[i].GetD3D12Resource().Get());
            m_backBufferTextures[i].Reset();
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        ThrowIfFailed(m_dxgiSwapChain->GetDesc(&swapChainDesc));
        ThrowIfFailed(m_dxgiSwapChain->ResizeBuffers(s_bufferCount, m_clientWidth,
            m_clientHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

        m_currentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

        UpdateRenderTargetViews();

        /*m_swapChain->Resize();*/
    }
}

bool Window::IsFullScreen() const
{
    return m_fullscreen;
}

void Window::SetFullscreen(bool a_fullscreen)
{
    assert(false);
    a_fullscreen;
}

int Window::GetClientWidth() const
{
    return m_clientWidth;
}

int Window::GetClientHeight() const
{
    return m_clientHeight;
}

HWND Window::GetWindowHandle() const
{
    return m_hWnd;
}

const std::wstring& Window::GetWindowName() const
{
    return m_windowName;
}

bool Window::IsVSyncEnabled() const
{
    return m_vSync;
}

void Window::SetVSync(bool a_vSync)
{
    m_vSync = a_vSync;
}

//UINT Window::GetCurrentBackBufferIndex() const
//{
//    return m_swapChain->m_currentBackBufferIndex;
//}

UINT Window::Present(const Texture& texture)
{
    auto commandQueue = DX12Renderer::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    auto commandList = commandQueue->GetCommandList();

    auto& backBuffer = m_backBufferTextures[m_currentBackBufferIndex];

    if (texture.IsValid())
    {
        if (texture.GetD3D12ResourceDesc().SampleDesc.Count > 1)
        {
            commandList->ResolveSubresource(backBuffer, texture);
        }
        else
        {
            commandList->CopyResource(backBuffer, texture);
        }
    }

    RenderTarget renderTarget;
    renderTarget.AttachTexture(AttachmentPoint::Color0, backBuffer);

    m_gui->Render(commandList, renderTarget);

    commandList->TransitionBarrier(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
    commandQueue->ExecuteCommandList(commandList);

    UINT syncInterval = m_vSync ? 1 : 0;
    UINT presentFlags = m_isTearingSupported && !m_vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(m_dxgiSwapChain->Present(syncInterval, presentFlags));

    m_fenceValues[m_currentBackBufferIndex] = commandQueue->Signal();
    m_frameValues[m_currentBackBufferIndex] = DX12Renderer::GetFrameCount();

    m_currentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

    commandQueue->WaitForFenceValue(m_fenceValues[m_currentBackBufferIndex]);

    DX12Renderer::GetInstance().ReleaseStaleDescriptors(m_frameValues[m_currentBackBufferIndex]);

    return m_currentBackBufferIndex;
}

void Window::Show()
{
    ::ShowWindow(m_hWnd, SW_SHOW);
}

void Window::Hide()
{
    ::ShowWindow(m_hWnd, SW_HIDE);
}

const RenderTarget& Window::GetRenderTarget() const
{
    m_renderTarget.AttachTexture(AttachmentPoint::Color0, m_backBufferTextures[m_currentBackBufferIndex]);
    return m_renderTarget;
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> Window::CreateSwapChain()
{
    ComPtr<IDXGISwapChain4> dxgiSwapChain4;
    ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_clientWidth;
    swapChainDesc.Height = m_clientHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc = { 1, 0 };
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = s_bufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    // It is recommended to always allow tearing if tearing support is available.
    swapChainDesc.Flags = m_isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    ID3D12CommandQueue* pCommandQueue = DX12Renderer::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetD3D12CommandQueue().Get();

    ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
        pCommandQueue,
        m_hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1));

    // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
    // will be handled manually.
    ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

    m_currentBackBufferIndex = dxgiSwapChain4->GetCurrentBackBufferIndex();

    return dxgiSwapChain4;
}

void Window::SetDisplayName(const std::string& a_displayName)
{
	m_displayName = a_displayName;
    SetWindowTextA(m_hWnd, m_displayName.c_str());
}

void Window::UpdateRenderTargetViews()
{
    for (int i = 0; i < s_bufferCount; ++i)
    {
        ComPtr<ID3D12Resource> backBuffer;
        ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

        ResourceStateTracker::AddGlobalResourceState(backBuffer.Get(), D3D12_RESOURCE_STATE_COMMON);

        m_backBufferTextures[i].SetD3D12Resource(backBuffer);
        m_backBufferTextures[i].CreateViews();
    }
}

void Window::SetRect(RECT a_rect)
{
    m_rect = a_rect;
}

//WindowGUI& Window::GetWindowGui()
//{
//    HE_CORE_ASSERT(m_gui != nullptr, "The window has not set a WindowGUI instance");
//}

void Window::RegisterWindowClass(const WNDCLASSEXW& windowClassData)
{
    if (!RegisterClassExW(&windowClassData))
    {
        MessageBoxA(NULL, "Unable to register the window class.", "Error", MB_OK | MB_ICONERROR);
    }
}

Window::Window(HINSTANCE a_hInstance, const std::wstring& a_windowClass, const std::wstring& a_windowTitle, uint32_t a_width, uint32_t a_height, bool a_vSync)
    : m_windowName(a_windowTitle)
    , m_clientWidth(a_width)
    , m_clientHeight(a_height)
    , m_vSync(a_vSync)
    , m_fullscreen(false)
    /*, m_FrameCounter(0)*/
{
    CreateWindowHandle(a_hInstance, a_windowClass.c_str());

    DX12Renderer& renderer = DX12Renderer::GetInstance();

    m_isTearingSupported = renderer.IsTearingSupported();

    for (int i = 0; i < s_bufferCount; ++i)
    {
        m_backBufferTextures[i].SetName(L"Backbuffer[" + std::to_wstring(i) + L"]");
    }

    m_dxgiSwapChain = CreateSwapChain();
    UpdateRenderTargetViews();

    m_gui = std::make_unique<GUI>(*this);
    bool result = m_gui->Initialize();
    result;
    HE_CORE_ASSERT(result, "Window did not successfully initialize imgui");
}

void Window::CreateWindowHandle(HINSTANCE a_hInstance, const wchar_t* a_windowClassName)
{
    RECT windowRect = { 0, 0, static_cast<LONG>(m_clientWidth), static_cast<LONG>(m_clientHeight) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWnd = CreateWindowW(a_windowClassName, m_windowName.c_str(),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, a_hInstance, nullptr);

    if (!m_hWnd)
    {
        MessageBoxA(NULL, "Could not create the render window.", "Error", MB_OK | MB_ICONERROR);
        assert(false);
    }
}