#pragma once

#include "Renderer/Window.h"

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

class SwapChain
{
public:
    SwapChain(Window& a_rWindow);
    ~SwapChain();

    void Resize();
    UINT Present();
private:
    void CreateSwapChain();
    void UpdateRenderTargetViews();
    void ResizeDepthBuffer();

public:
    //reference to parent
    Window& m_rWindow;

    //swap chain and corresponding backbuffers/RTV descriptor heap
    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgiSwapChain;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_backBuffers[Window::s_bufferCount];
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;

    UINT m_RTVDescriptorSize;
    UINT m_currentBackBufferIndex;

    uint64_t m_fenceValues[Window::s_bufferCount] = { 0 };

    //depth buffer and corresponding descriptor heap
    Microsoft::WRL::ComPtr<ID3D12Resource> m_depthBuffer;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dSVHeap;
};

