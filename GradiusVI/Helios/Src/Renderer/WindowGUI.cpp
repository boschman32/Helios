#include "hepch.h"
#include "Renderer/WindowGUI.h"

#include "Renderer/DX12Renderer.h"
#include "Renderer/Window.h"
#include "Renderer/imgui_impl_win32.h"
#include "Renderer/CommandQueue.h"
#include "Renderer/RenderHelpers.h"

#include "Core/Core.h"

#include <d3dcompiler.h>

WindowGUI::WindowGUI(Window& a_window, int a_bufferCount)
    : m_parent(a_window)
    , m_bufferCount(a_bufferCount)
{

}

WindowGUI::~WindowGUI()
{

}

//bool WindowGUI::Initialize()
//{
//    DX12Renderer& renderer = DX12Renderer::GetInstance();
//    auto device = renderer.GetDevice();
//
//    //consider creating context outside of this function
//    m_pImGuiContext = ImGui::CreateContext();
//    ImGui::SetCurrentContext(m_pImGuiContext);
//
//    if (!ImGui_ImplWin32_Init(m_parent.GetWindowHandle()))
//    {
//        return false;
//    }
//
//    ImGuiIO& io = ImGui::GetIO();
//
//    io.BackendRendererName = "imgui_impl_dx12";
//    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
//
//    m_RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM; //TODO: consider this being set based on window
//
//    /*m_pd3dSrvDescHeap = renderer.CreateDescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);*/
//    {
//        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//        desc.NumDescriptors = 1;
//        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//        if (device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3dSrvDescHeap)) != S_OK)
//            return false;
//    }
//
//    m_hFontSrvCpuDescHandle = m_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
//    m_hFontSrvGpuDescHandle = m_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();
//    
//    for (int i = 0; i < m_bufferCount; i++)
//    {
//        m_FrameResources.emplace_back();
//        m_FrameResources.back().IndexBuffer = nullptr;
//        m_FrameResources.back().VertexBuffer = nullptr;
//        m_FrameResources.back().IndexBufferSize = 10000;
//        m_FrameResources.back().VertexBufferSize = 5000;
//    }
//
//    io.Fonts->AddFontDefault();
//    /*io.Fonts->AddFontFromFileTTF("D:/OpenSans-Regular.ttf", 16.0f);*/
//
//    return true;
//}
//
//void WindowGUI::NewFrame()
//{
//    if (m_pPipelineState == nullptr)
//    {
//        CreateDeviceObjects();
//    }
//
//    ImGui_ImplWin32_NewFrame();
//    ImGui::NewFrame();
//}
//
//void WindowGUI::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> a_commandList)
//{
//    ID3D12DescriptorHeap* descriptorHeap[] = { m_pd3dSrvDescHeap.Get() };
//    a_commandList->SetDescriptorHeaps(1, descriptorHeap);
//
//    ImGui::SetCurrentContext(m_pImGuiContext);
//    ImGui::Render();
//
//    RenderDrawData(ImGui::GetDrawData(), a_commandList);
//}
//
//bool WindowGUI::CreateDeviceObjects()
//{
//    DX12Renderer& renderer = DX12Renderer::GetInstance();
//    auto device = renderer.GetDevice();
//
//    {
//        D3D12_DESCRIPTOR_RANGE descRange = {};
//        descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//        descRange.NumDescriptors = 1;
//        descRange.BaseShaderRegister = 0;
//        descRange.RegisterSpace = 0;
//        descRange.OffsetInDescriptorsFromTableStart = 0;
//
//        D3D12_ROOT_PARAMETER param[2] = {};
//
//        param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
//        param[0].Constants.ShaderRegister = 0;
//        param[0].Constants.RegisterSpace = 0;
//        param[0].Constants.Num32BitValues = 16;
//        param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
//
//        param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//        param[1].DescriptorTable.NumDescriptorRanges = 1;
//        param[1].DescriptorTable.pDescriptorRanges = &descRange;
//        param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//
//        D3D12_STATIC_SAMPLER_DESC staticSampler = {};
//        staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
//        staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//        staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//        staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//        staticSampler.MipLODBias = 0.f;
//        staticSampler.MaxAnisotropy = 0;
//        staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
//        staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
//        staticSampler.MinLOD = 0.f;
//        staticSampler.MaxLOD = 0.f;
//        staticSampler.ShaderRegister = 0;
//        staticSampler.RegisterSpace = 0;
//        staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//
//        D3D12_ROOT_SIGNATURE_DESC desc = {};
//        desc.NumParameters = _countof(param);
//        desc.pParameters = param;
//        desc.NumStaticSamplers = 1;
//        desc.pStaticSamplers = &staticSampler;
//        desc.Flags =
//            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
//            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
//            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
//            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
//
//        ID3DBlob* blob = NULL;
//        if (D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, NULL) != S_OK)
//        {
//            return false;
//        }     
//
//        device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
//        blob->Release();
//    }
//
//    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
//    memset(&psoDesc, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
//    psoDesc.NodeMask = 1;
//    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//    psoDesc.pRootSignature = m_pRootSignature.Get();
//    psoDesc.SampleMask = UINT_MAX;
//    psoDesc.NumRenderTargets = 1;
//    psoDesc.RTVFormats[0] = m_RTVFormat;
//    psoDesc.SampleDesc.Count = 1;
//    psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
//
//    // Create the vertex shader
//    {
//        static const char* vertexShader =
//            "cbuffer vertexBuffer : register(b0) \
//            {\
//              float4x4 ProjectionMatrix; \
//            };\
//            struct VS_INPUT\
//            {\
//              float2 pos : POSITION;\
//              float4 col : COLOR0;\
//              float2 uv  : TEXCOORD0;\
//            };\
//            \
//            struct PS_INPUT\
//            {\
//              float4 pos : SV_POSITION;\
//              float4 col : COLOR0;\
//              float2 uv  : TEXCOORD0;\
//            };\
//            \
//            PS_INPUT main(VS_INPUT input)\
//            {\
//              PS_INPUT output;\
//              output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
//              output.col = input.col;\
//              output.uv  = input.uv;\
//              return output;\
//            }";
//
//        D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_5_0", 0, 0, &m_pVertexShaderBlob, NULL);
//        if (m_pVertexShaderBlob == NULL) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
//            return false;
//        psoDesc.VS = { m_pVertexShaderBlob->GetBufferPointer(), m_pVertexShaderBlob->GetBufferSize() };
//
//        // Create the input layout
//        static D3D12_INPUT_ELEMENT_DESC local_layout[] = {
//            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)IM_OFFSETOF(ImDrawVert, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)IM_OFFSETOF(ImDrawVert, uv),  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//            { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)IM_OFFSETOF(ImDrawVert, col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        };
//        psoDesc.InputLayout = { local_layout, 3 };
//    }
//
//    // Create the pixel shader
//    {
//        static const char* pixelShader =
//            "struct PS_INPUT\
//            {\
//              float4 pos : SV_POSITION;\
//              float4 col : COLOR0;\
//              float2 uv  : TEXCOORD0;\
//            };\
//            SamplerState sampler0 : register(s0);\
//            Texture2D texture0 : register(t0);\
//            \
//            float4 main(PS_INPUT input) : SV_Target\
//            {\
//              float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
//              return out_col; \
//            }";
//
//        D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_5_0", 0, 0, &m_pPixelShaderBlob, NULL);
//        if (m_pPixelShaderBlob == NULL)  // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
//            return false;
//        psoDesc.PS = { m_pPixelShaderBlob->GetBufferPointer(), m_pPixelShaderBlob->GetBufferSize() };
//    }
//
//    // Create the blending setup
//    {
//        D3D12_BLEND_DESC& desc = psoDesc.BlendState;
//        desc.AlphaToCoverageEnable = false;
//        desc.RenderTarget[0].BlendEnable = true;
//        desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
//        desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
//        desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
//        desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
//        desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
//        desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
//        desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//    }
//
//    // Create the rasterizer state
//    {
//        D3D12_RASTERIZER_DESC& desc = psoDesc.RasterizerState;
//        desc.FillMode = D3D12_FILL_MODE_SOLID;
//        desc.CullMode = D3D12_CULL_MODE_NONE;
//        desc.FrontCounterClockwise = FALSE;
//        desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
//        desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
//        desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
//        desc.DepthClipEnable = true;
//        desc.MultisampleEnable = FALSE;
//        desc.AntialiasedLineEnable = FALSE;
//        desc.ForcedSampleCount = 0;
//        desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
//    }
//
//    // Create depth-stencil State
//    {
//        D3D12_DEPTH_STENCIL_DESC& desc = psoDesc.DepthStencilState;
//        desc.DepthEnable = false;
//        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//        desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
//        desc.StencilEnable = false;
//        desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
//        desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
//        desc.BackFace = desc.FrontFace;
//    }
//
//    if (device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPipelineState)) != S_OK)
//        return false;
//
//    CreateFontTexture();
//
//    return true;
//}
//
//void WindowGUI::CreateFontTexture()
//{
//    DX12Renderer& renderer = DX12Renderer::GetInstance();
//    auto device = renderer.GetDevice();
//
//    // Build texture atlas
//    ImGuiIO& io = ImGui::GetIO();
//    unsigned char* pixels;
//    int width, height;
//    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
//
//    // Upload texture to graphics system
//    {
//        D3D12_HEAP_PROPERTIES props;
//        memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
//        props.Type = D3D12_HEAP_TYPE_DEFAULT;
//        props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
//        props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
//
//        D3D12_RESOURCE_DESC desc;
//        ZeroMemory(&desc, sizeof(desc));
//        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//        desc.Alignment = 0;
//        desc.Width = width;
//        desc.Height = height;
//        desc.DepthOrArraySize = 1;
//        desc.MipLevels = 1;
//        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//        desc.SampleDesc.Count = 1;
//        desc.SampleDesc.Quality = 0;
//        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
//
//        ID3D12Resource* pTexture = NULL;
//        ThrowIfFailed(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
//            D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&pTexture)));
//
//        UINT uploadPitch = (width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
//        UINT uploadSize = height * uploadPitch;
//        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//        desc.Alignment = 0;
//        desc.Width = uploadSize;
//        desc.Height = 1;
//        desc.DepthOrArraySize = 1;
//        desc.MipLevels = 1;
//        desc.Format = DXGI_FORMAT_UNKNOWN;
//        desc.SampleDesc.Count = 1;
//        desc.SampleDesc.Quality = 0;
//        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
//
//        props.Type = D3D12_HEAP_TYPE_UPLOAD;
//        props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
//        props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
//
//        ID3D12Resource* uploadBuffer = NULL;
//        ThrowIfFailed(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
//            D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer)));
//
//        void* mapped = NULL;
//        D3D12_RANGE range = { 0, uploadSize };
//        ThrowIfFailed(uploadBuffer->Map(0, &range, &mapped));
//        for (int y = 0; y < height; y++)
//            memcpy((void*)((uintptr_t)mapped + y * uploadPitch), pixels + y * width * 4, width * 4);
//        uploadBuffer->Unmap(0, &range);
//
//        D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
//        srcLocation.pResource = uploadBuffer;
//        srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
//        srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//        srcLocation.PlacedFootprint.Footprint.Width = width;
//        srcLocation.PlacedFootprint.Footprint.Height = height;
//        srcLocation.PlacedFootprint.Footprint.Depth = 1;
//        srcLocation.PlacedFootprint.Footprint.RowPitch = uploadPitch;
//
//        D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
//        dstLocation.pResource = pTexture;
//        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
//        dstLocation.SubresourceIndex = 0;
//
//        D3D12_RESOURCE_BARRIER barrier = {};
//        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//        barrier.Transition.pResource = pTexture;
//        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
//        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
//        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
//
//        /*ID3D12Fence* fence = NULL;
//        ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));*/
//
//        /*HANDLE event = CreateEvent(0, 0, 0, 0);
//        IM_ASSERT(event != NULL);*/
//
//        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
//        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//        queueDesc.NodeMask = 1;
//
//        auto CommandQueue = renderer.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
//        auto CommandList = CommandQueue->GetD3D12CommandList();
//
//        CommandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, NULL);
//        CommandList->ResourceBarrier(1, &barrier);
//
//        CommandQueue->ExecuteCommandList(CommandList);
//
//        uploadBuffer->Release();
//
//        // Create texture view
//        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
//        ZeroMemory(&srvDesc, sizeof(srvDesc));
//        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//        srvDesc.Texture2D.MipLevels = desc.MipLevels;
//        srvDesc.Texture2D.MostDetailedMip = 0;
//        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//        device->CreateShaderResourceView(pTexture, &srvDesc, m_hFontSrvCpuDescHandle);
//
//        m_pFontTextureResource = pTexture;
//    }
//
//    // Store our identifier
////#pragma warning(push)
////#pragma warning(disable : 4127)
////    HE_CORE_ASSERT(sizeof(ImTextureID) >= sizeof(m_hFontSrvGpuDescHandle.ptr), "Can't pack descriptor handle into TexID, 32-bit not supported yet.");
////#pragma warning(pop)
//
//    io.Fonts->TexID = (ImTextureID)m_hFontSrvGpuDescHandle.ptr;
//}
//
//void WindowGUI::RenderDrawData(ImDrawData* a_draw_data, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> a_ctx)
//{
//    DX12Renderer& renderer = DX12Renderer::GetInstance();
//    auto device = renderer.GetDevice();
//
//    // Avoid rendering when minimized
//    if (a_draw_data->DisplaySize.x <= 0.0f || a_draw_data->DisplaySize.y <= 0.0f)
//        return;
//
//    // FIXME: I'm assuming that this only gets called once per frame!
//    // If not, we can't just re-allocate the IB or VB, we'll have to do a proper allocator.
//    FrameResources* fr = &m_FrameResources[m_parent.GetCurrentBackBufferIndex()];
//    
//    // Create and grow vertex/index buffers if needed
//    if (fr->VertexBuffer == nullptr || fr->VertexBufferSize < a_draw_data->TotalVtxCount)
//    {
//        fr->VertexBufferSize = a_draw_data->TotalVtxCount + 5000;
//        D3D12_HEAP_PROPERTIES props;
//        memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
//        props.Type = D3D12_HEAP_TYPE_UPLOAD;
//        props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
//        props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
//        D3D12_RESOURCE_DESC desc;
//        memset(&desc, 0, sizeof(D3D12_RESOURCE_DESC));
//        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//        desc.Width = fr->VertexBufferSize * sizeof(ImDrawVert);
//        desc.Height = 1;
//        desc.DepthOrArraySize = 1;
//        desc.MipLevels = 1;
//        desc.Format = DXGI_FORMAT_UNKNOWN;
//        desc.SampleDesc.Count = 1;
//        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
//        if (device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&fr->VertexBuffer)) < 0)
//            return;
//    }
//    if (fr->IndexBuffer == nullptr || fr->IndexBufferSize < a_draw_data->TotalIdxCount)
//    {
//        fr->IndexBufferSize = a_draw_data->TotalIdxCount + 10000;
//        D3D12_HEAP_PROPERTIES props;
//        memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
//        props.Type = D3D12_HEAP_TYPE_UPLOAD;
//        props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
//        props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
//        D3D12_RESOURCE_DESC desc;
//        memset(&desc, 0, sizeof(D3D12_RESOURCE_DESC));
//        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//        desc.Width = fr->IndexBufferSize * sizeof(ImDrawIdx);
//        desc.Height = 1;
//        desc.DepthOrArraySize = 1;
//        desc.MipLevels = 1;
//        desc.Format = DXGI_FORMAT_UNKNOWN;
//        desc.SampleDesc.Count = 1;
//        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
//        if (device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&fr->IndexBuffer)) < 0)
//            return;
//    }
//
//    // Upload vertex/index data into a single contiguous GPU buffer
//    void* vtx_resource, * idx_resource;
//    D3D12_RANGE range;
//    memset(&range, 0, sizeof(D3D12_RANGE));
//    if (fr->VertexBuffer->Map(0, &range, &vtx_resource) != S_OK)
//        return;
//    if (fr->IndexBuffer->Map(0, &range, &idx_resource) != S_OK)
//        return;
//    ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource;
//    ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource;
//    for (int n = 0; n < a_draw_data->CmdListsCount; n++)
//    {
//        const ImDrawList* cmd_list = a_draw_data->CmdLists[n];
//        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
//        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
//        vtx_dst += cmd_list->VtxBuffer.Size;
//        idx_dst += cmd_list->IdxBuffer.Size;
//    }
//    fr->VertexBuffer->Unmap(0, &range);
//    fr->IndexBuffer->Unmap(0, &range);
//
//    // Setup desired DX state
//    SetupRenderState(a_draw_data, a_ctx, fr);
//
//    // Render command lists
//    // (Because we merged all buffers into a single one, we maintain our own offset into them)
//    int global_vtx_offset = 0;
//    int global_idx_offset = 0;
//    ImVec2 clip_off = a_draw_data->DisplayPos;
//    for (int n = 0; n < a_draw_data->CmdListsCount; n++)
//    {
//        const ImDrawList* cmd_list = a_draw_data->CmdLists[n];
//        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
//        {
//            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
//            if (pcmd->UserCallback != NULL)
//            {
//                // User callback, registered via ImDrawList::AddCallback()
//                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
//                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
//                    SetupRenderState(a_draw_data, a_ctx, fr);
//                else
//                    pcmd->UserCallback(cmd_list, pcmd);
//            }
//            else
//            {
//                // Apply Scissor, Bind texture, Draw
//                const D3D12_RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
//                
//                D3D12_GPU_DESCRIPTOR_HANDLE test = { m_hFontSrvGpuDescHandle.ptr };
//                a_ctx->SetGraphicsRootDescriptorTable(1, test);
//
//                /*a_ctx->SetGraphicsRootDescriptorTable(1, *(D3D12_GPU_DESCRIPTOR_HANDLE*)& pcmd->TextureId);*/
//                a_ctx->RSSetScissorRects(1, &r);
//                a_ctx->DrawIndexedInstanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
//            }
//        }
//        global_idx_offset += cmd_list->IdxBuffer.Size;
//        global_vtx_offset += cmd_list->VtxBuffer.Size;
//    }
//}
//
//void WindowGUI::SetupRenderState(ImDrawData* a_draw_data, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> a_ctx, FrameResources* fr)
//{
//    // Setup orthographic projection matrix into our constant buffer
//    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
//    VERTEX_CONSTANT_BUFFER vertex_constant_buffer;
//    {
//        float L = a_draw_data->DisplayPos.x;
//        float R = a_draw_data->DisplayPos.x + a_draw_data->DisplaySize.x;
//        float T = a_draw_data->DisplayPos.y;
//        float B = a_draw_data->DisplayPos.y + a_draw_data->DisplaySize.y;
//        float mvp[4][4] =
//        {
//            { 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
//            { 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
//            { 0.0f,         0.0f,           0.5f,       0.0f },
//            { (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
//        };
//        memcpy(&vertex_constant_buffer.mvp, mvp, sizeof(mvp));
//    }
//
//    // Setup viewport
//    D3D12_VIEWPORT vp;
//    memset(&vp, 0, sizeof(D3D12_VIEWPORT));
//    vp.Width = a_draw_data->DisplaySize.x;
//    vp.Height = a_draw_data->DisplaySize.y;
//    vp.MinDepth = 0.0f;
//    vp.MaxDepth = 1.0f;
//    vp.TopLeftX = vp.TopLeftY = 0.0f;
//    a_ctx->RSSetViewports(1, &vp);
//
//    // Bind shader and vertex buffers
//    unsigned int stride = sizeof(ImDrawVert);
//    unsigned int offset = 0;
//    D3D12_VERTEX_BUFFER_VIEW vbv;
//    memset(&vbv, 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));
//    vbv.BufferLocation = fr->VertexBuffer->GetGPUVirtualAddress() + offset;
//    vbv.SizeInBytes = fr->VertexBufferSize * stride;
//    vbv.StrideInBytes = stride;
//    a_ctx->IASetVertexBuffers(0, 1, &vbv);
//    D3D12_INDEX_BUFFER_VIEW ibv;
//    memset(&ibv, 0, sizeof(D3D12_INDEX_BUFFER_VIEW));
//    ibv.BufferLocation = fr->IndexBuffer->GetGPUVirtualAddress();
//    ibv.SizeInBytes = fr->IndexBufferSize * sizeof(ImDrawIdx);
//    ibv.Format = sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
//    a_ctx->IASetIndexBuffer(&ibv);
//    a_ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//    a_ctx->SetPipelineState(m_pPipelineState.Get());
//    a_ctx->SetGraphicsRootSignature(m_pRootSignature.Get());
//    a_ctx->SetGraphicsRoot32BitConstants(0, 16, &vertex_constant_buffer, 0);
//
//    // Setup blend factor
//    const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
//    a_ctx->OMSetBlendFactor(blend_factor);
//}
