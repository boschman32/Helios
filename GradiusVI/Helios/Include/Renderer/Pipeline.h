#pragma once

#include "Renderer/Camera.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Light.h"
#include "Renderer/Window.h"
#include "Renderer/Mesh.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/RootSignature.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Material.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <DirectXMath.h>

class Window;
namespace Helios { class Entity; }

class Pipeline
{
public:
    Pipeline(uint32_t a_width = 1, uint32_t a_height = 1);
    ~Pipeline();

    void RenderScene(Window& a_rWindow, DirectX::XMFLOAT4 viewportRanges, std::vector<Helios::Entity*>& pEntities);

    void RescaleHDRRenderTarget(float scale);
    void OnResize(uint32_t a_width, uint32_t a_height);

    void LoadContent(uint32_t a_width = 1, uint32_t a_height = 1);

public:
    friend class DX12Renderer;
    friend class Mesh;
    
private:
    bool m_isContentLoaded = false;

    // Some geometry to render.
    std::unique_ptr<Mesh> m_cubeMesh;
    std::unique_ptr<Mesh> m_sphereMesh;
    std::unique_ptr<Mesh> m_coneMesh;
    std::unique_ptr<Mesh> m_torusMesh;
    std::unique_ptr<Mesh> m_planeMesh;

    std::unique_ptr<Mesh> m_UIQuadMesh;

    std::unique_ptr<Mesh> m_skyboxMesh;

    Texture m_defaultTexture;

    Texture m_spaceTexture;
    Texture m_spaceCubemap;

    // HDR Render target
    RenderTarget m_hDRRenderTarget;

    // Root signatures
    RootSignature m_skyboxSignature;
    RootSignature m_hDRRootSignature;
    RootSignature m_sDRRootSignature;
    RootSignature m_UIRootSignature;

    // Pipeline state object.
    // Skybox PSO
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_skyboxPipelineState;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_hDRPipelineState;
    // HDR -> SDR tone mapping PSO.
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_sDRPipelineState;
    //UI rendering
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_UIPipelineState;

    D3D12_RECT m_scissorRect;

    Camera m_camera;
    Camera m_UICamera;
    struct alignas(16) CameraData //TODO: remove m_
    {
        DirectX::XMVECTOR m_initialCamPos;
        DirectX::XMVECTOR m_initialCamRot;
        float m_initialFov;
    };
    CameraData* m_pAlignedCameraData;

    // Camera controller
    float m_forward;
    float m_backward;
    float m_left;
    float m_right;
    float m_up;
    float m_down;

    float m_pitch;
    float m_yaw;

    // Rotate the lights in a circle.
    bool m_animateLights;
    // Set to true if the Shift key is pressed.
    bool m_shift;

    int m_width; //TODO: remove this. size should be fetched from window, and each window should have its own detph buffer
    int m_height;

    // Scale the HDR render target to a fraction of the window size.
    float m_renderScale;

    // Define some lights.
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
};