#pragma once

#include "imgui.h"
class Window;

#include <memory>
#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h> //consider switching this to dxgi_4
#include <wrl.h>

struct FrameResources
{
    Microsoft::WRL::ComPtr<ID3D12Resource> IndexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBuffer;
    int                 IndexBufferSize;
    int                 VertexBufferSize;
};

struct VERTEX_CONSTANT_BUFFER
{
    float   mvp[4][4];
};

class WindowGUI
{
public:
    WindowGUI(Window& a_window, int a_bufferCount);
    ~WindowGUI();

    bool Initialize();

    void NewFrame();
    void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> a_commandList);

private:
    bool CreateDeviceObjects();
    void CreateFontTexture();

    void RenderDrawData(ImDrawData* a_draw_data, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> a_ctx);
    void SetupRenderState(ImDrawData* a_draw_data, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> a_ctx, FrameResources* fr);


public:

private:
    Window& m_parent;

    ImGuiContext* m_pImGuiContext = nullptr;

    DXGI_FORMAT m_RTVFormat = DXGI_FORMAT_UNKNOWN;
    D3D12_CPU_DESCRIPTOR_HANDLE  m_hFontSrvCpuDescHandle = {};
    D3D12_GPU_DESCRIPTOR_HANDLE  m_hFontSrvGpuDescHandle = {};
    Microsoft::WRL::ComPtr<ID3D12Resource> m_pFontTextureResource;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pd3dSrvDescHeap;

    int m_bufferCount;
    std::vector<FrameResources> m_FrameResources;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;

    Microsoft::WRL::ComPtr<ID3D10Blob> m_pVertexShaderBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3D10Blob> m_pPixelShaderBlob = nullptr;
};

