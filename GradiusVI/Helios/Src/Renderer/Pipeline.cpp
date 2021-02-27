#include "hepch.h"

#include "Renderer/Pipeline.h"
#include "Renderer/DX12Renderer.h"
#include "Renderer/CommandQueue.h"
#include "Renderer/CommandList.h"
#include "Renderer/RenderHelpers.h"
#include "Renderer/Window.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Renderer/WindowGUI.h"
#include "Renderer/UIQuadData.h"

#include <Core/Core.h>

#include "Renderer/d3dx12.h"

#include <d3dcompiler.h>
#include <algorithm>

#include <Windows.h>

#include <DirectXColors.h>

#include <Core/EntityComponent/Entity.h>

#include "../Shaders/Headers/Sprite_vs.h"
#include "../Shaders/Headers/Sprite_ps.h"
#include "Core/CollisionManager.h"

using namespace Microsoft::WRL;
using namespace DirectX;

struct Mat
{
    XMMATRIX ModelMatrix;
    XMMATRIX ModelViewMatrix;
    XMMATRIX InverseTransposeModelViewMatrix;
    XMMATRIX ModelViewProjectionMatrix;
};

struct LightProperties
{
    uint32_t NumPointLights;
    uint32_t NumSpotLights;
};

enum TonemapMethod : uint32_t
{
    TM_Linear,
    TM_Reinhard,
    TM_ReinhardSq,
    TM_ACESFilmic,
};

struct TonemapParameters
{
    TonemapParameters()
        : TonemapMethod(TM_Reinhard)
        , Exposure(0.0f)
        , MaxLuminance(1.0f)
        , K(1.0f)
        , A(0.22f)
        , B(0.3f)
        , C(0.1f)
        , D(0.2f)
        , E(0.01f)
        , F(0.3f)
        , LinearWhite(11.2f)
        , Gamma(2.2f)
    {}

    // The method to use to perform tonemapping.
    TonemapMethod TonemapMethod;
    // Exposure should be expressed as a relative exposure value (-2, -1, 0, +1, +2 )
    float Exposure;

    // The maximum luminance to use for linear tonemapping.
    float MaxLuminance;

    // Reinhard constant. Generally this is 1.0.
    float K;

    // ACES Filmic parameters
    // See: https://www.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting/142
    float A; // Shoulder strength
    float B; // Linear strength
    float C; // Linear angle
    float D; // Toe strength
    float E; // Toe Numerator
    float F; // Toe denominator
    // Note E/F = Toe angle.
    float LinearWhite;
    float Gamma;
};

TonemapParameters g_TonemapParameters;

// An enum for root signature parameters.
// I'm not using scoped enums to avoid the explicit cast that would be required
// to use these as root indices in the root signature.
enum RootParameters
{
    MatricesCB,         // ConstantBuffer<Mat> MatCB : register(b0);
    MaterialCB,         // ConstantBuffer<Material> MaterialCB : register( b0, space1 );
    LightPropertiesCB,  // ConstantBuffer<LightProperties> LightPropertiesCB : register( b1 );
    PointLights,        // StructuredBuffer<PointLight> PointLights : register( t0 );
    SpotLights,         // StructuredBuffer<SpotLight> SpotLights : register( t1 );
    Textures,           // Texture2D DiffuseTexture : register( t2 );
    NumRootParameters
};

// Builds a look-at (world) matrix from a point, up and direction vectors.
XMMATRIX XM_CALLCONV LookAtMatrix(FXMVECTOR Position, FXMVECTOR Direction, FXMVECTOR Up)
{
    assert(!XMVector3Equal(Direction, XMVectorZero()));
    assert(!XMVector3IsInfinite(Direction));
    assert(!XMVector3Equal(Up, XMVectorZero()));
    assert(!XMVector3IsInfinite(Up));

    XMVECTOR R2 = XMVector3Normalize(Direction);

    XMVECTOR R0 = XMVector3Cross(Up, R2);
    R0 = XMVector3Normalize(R0);

    XMVECTOR R1 = XMVector3Cross(R2, R0);

    XMMATRIX M(R0, R1, R2, Position);

    return M;
}

// Number of values to plot in the tonemapping curves.
static const int VALUES_COUNT = 256;
// Maximum HDR value to normalize the plot samples.
static const float HDR_MAX = 12.0f;

float LinearTonemapping(float HDR, float max)
{
    if (max > 0.0f)
    {
        return std::clamp(HDR / max, 0.0f, 0.1f);
    }
    return HDR;
}

float LinearTonemappingPlot(void*, int index)
{
    return LinearTonemapping(index / (float)VALUES_COUNT * HDR_MAX, g_TonemapParameters.MaxLuminance);
}

// Reinhard tone mapping.
// See: http://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf
float ReinhardTonemapping(float HDR, float k)
{
    return HDR / (HDR + k);
}

float ReinhardTonemappingPlot(void*, int index)
{
    return ReinhardTonemapping(index / (float)VALUES_COUNT * HDR_MAX, g_TonemapParameters.K);
}

float ReinhardSqrTonemappingPlot(void*, int index)
{
    float reinhard = ReinhardTonemapping(index / (float)VALUES_COUNT * HDR_MAX, g_TonemapParameters.K);
    return reinhard * reinhard;
}

// ACES Filmic
// See: https://www.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting/142
float ACESFilmicTonemapping(float x, float A, float B, float C, float D, float E, float F)
{
    return (((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - (E / F));
}

float ACESFilmicTonemappingPlot(void*, int index)
{
    float HDR = index / (float)VALUES_COUNT * HDR_MAX;
    return ACESFilmicTonemapping(HDR, g_TonemapParameters.A, g_TonemapParameters.B, g_TonemapParameters.C, g_TonemapParameters.D, g_TonemapParameters.E, g_TonemapParameters.F) /
        ACESFilmicTonemapping(g_TonemapParameters.LinearWhite, g_TonemapParameters.A, g_TonemapParameters.B, g_TonemapParameters.C, g_TonemapParameters.D, g_TonemapParameters.E, g_TonemapParameters.F);
}

void XM_CALLCONV ComputeMatrices(FXMMATRIX model, CXMMATRIX view, CXMMATRIX viewProjection, Mat& mat)
{
    mat.ModelMatrix = model;
    mat.ModelViewMatrix = model * view;
    mat.InverseTransposeModelViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, mat.ModelViewMatrix));
    mat.ModelViewProjectionMatrix = model * viewProjection;
}

void Pipeline::RenderScene(Window& a_rWindow, DirectX::XMFLOAT4 viewportRanges, std::vector<Helios::Entity*>& pEntities)
{
    pEntities;
    viewportRanges;
    //resize to ensure that size of detph buffer is same as swapchain of window
    OnResize(a_rWindow.m_clientWidth, a_rWindow.m_clientHeight);

    //update
    {
        float speedMultipler = (m_shift ? 16.0f : 4.0f);

        XMVECTOR cameraPos = XMVectorSet(25, -20, -60, 1);
        XMVECTOR cameraTarget = XMVectorSet(25, -20, 0, 1);
        XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

        m_camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
        m_camera.set_Projection(45.0f, static_cast<float>(a_rWindow.m_clientWidth) / static_cast<float>(a_rWindow.m_clientHeight), 0.1f, 10000.0f);

        XMVECTOR cameraTranslate = XMVectorSet(m_right - m_left, 0.0f, m_forward - m_backward, 1.0f) * speedMultipler;
        XMVECTOR cameraPan = XMVectorSet(0.0f, m_up - m_down, 0.0f, 1.0f) * speedMultipler;
        m_camera.Translate(cameraTranslate, Space::Local);
        m_camera.Translate(cameraPan, Space::Local);

        XMVECTOR cameraRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_pitch), XMConvertToRadians(m_yaw), 0.0f);
        m_camera.set_Rotation(cameraRotation);

        XMMATRIX viewMatrix = m_camera.get_ViewMatrix();

        const int numPointLights = 4;
        const int numSpotLights = 4;

        static const XMVECTORF32 LightColors[] =
        {
            Colors::White, Colors::Orange, Colors::Yellow, Colors::Green, Colors::Blue, Colors::Indigo, Colors::Violet, Colors::White
        };

        static float lightAnimTime = 0.0f;
        if (m_animateLights)
        {
            lightAnimTime += 0.5f * XM_PI;
        }

        const float radius = 8.0f;
        const float offset = 2.0f * XM_PI / numPointLights;
        const float offset2 = offset + (offset / 2.0f);

        // Setup the light buffers.
        m_pointLights.resize(numPointLights);
        for (int i = 0; i < numPointLights; ++i)
        {
            PointLight& l = m_pointLights[i];

            l.PositionWS = {
                static_cast<float>(std::sin(lightAnimTime + offset * i))* radius,
                9.0f,
                static_cast<float>(std::cos(lightAnimTime + offset * i))* radius,
                1.0f
            };
            XMVECTOR positionWS = XMLoadFloat4(&l.PositionWS);
            XMVECTOR positionVS = XMVector3TransformCoord(positionWS, viewMatrix);
            XMStoreFloat4(&l.PositionVS, positionVS);

            l.Color = XMFLOAT4(LightColors[i]);
            l.Intensity = 1.0f;
            l.Attenuation = 0.0f;
        }

        m_spotLights.resize(numSpotLights);
        for (int i = 0; i < numSpotLights; ++i)
        {
            SpotLight& l = m_spotLights[i];

            l.PositionWS = {
                static_cast<float>(std::sin(lightAnimTime + offset * i + offset2))* radius,
                9.0f,
                static_cast<float>(std::cos(lightAnimTime + offset * i + offset2))* radius,
                1.0f
            };
            XMVECTOR positionWS = XMLoadFloat4(&l.PositionWS);
            XMVECTOR positionVS = XMVector3TransformCoord(positionWS, viewMatrix);
            XMStoreFloat4(&l.PositionVS, positionVS);

            XMVECTOR directionWS = XMVector3Normalize(XMVectorSetW(XMVectorNegate(positionWS), 0));
            XMVECTOR directionVS = XMVector3Normalize(XMVector3TransformNormal(directionWS, viewMatrix));
            XMStoreFloat4(&l.DirectionWS, directionWS);
            XMStoreFloat4(&l.DirectionVS, directionVS);

            l.Color = XMFLOAT4(LightColors[numPointLights + i]);
            l.Intensity = 1.0f;
            l.SpotAngle = XMConvertToRadians(45.0f);
            l.Attenuation = 0.0f;
        }
    }

    //render
    {
        auto commandQueue = DX12Renderer::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
        auto commandList = commandQueue->GetCommandList();

        // Clear the render targets.
        {
            FLOAT clearColor[] = { 1.0f, 0.2f, 0.2f, 1.0f };

            commandList->ClearTexture(m_hDRRenderTarget.GetTexture(AttachmentPoint::Color0), clearColor);
            commandList->ClearDepthStencilTexture(m_hDRRenderTarget.GetTexture(AttachmentPoint::DepthStencil), D3D12_CLEAR_FLAG_DEPTH);
        }

        commandList->SetRenderTarget(m_hDRRenderTarget);
        commandList->SetViewport(m_hDRRenderTarget.GetViewport());
        commandList->SetScissorRect(m_scissorRect);

        // Render the skybox.
        {
            // The view matrix should only consider the camera's rotation, but not the translation.
            static float skyboxRotation = 0.0f;
            skyboxRotation -= 0.026f;
            XMMATRIX rotationmatrix = XMMatrixRotationY(Math::Radians(skyboxRotation));
            auto viewMatrix = XMMatrixTranspose(XMMatrixRotationQuaternion(m_camera.get_Rotation()));
            auto projMatrix = m_camera.get_ProjectionMatrix();
            auto viewProjMatrix = rotationmatrix * viewMatrix * projMatrix;

            commandList->SetPipelineState(m_skyboxPipelineState);
            commandList->SetGraphicsRootSignature(m_skyboxSignature);

            commandList->SetGraphics32BitConstants(0, viewProjMatrix);

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = m_spaceCubemap.GetD3D12ResourceDesc().Format;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MipLevels = (UINT)-1; // Use all mips.

            // TODO: Need a better way to bind a cubemap.
            commandList->SetShaderResourceView(1, 0, m_spaceCubemap, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, &srvDesc);

            m_skyboxMesh->Render(*commandList);
        }


        commandList->SetPipelineState(m_hDRPipelineState);
        commandList->SetGraphicsRootSignature(m_hDRRootSignature);

        // Upload lights
        LightProperties lightProps;
        lightProps.NumPointLights = static_cast<uint32_t>(m_pointLights.size());
        lightProps.NumSpotLights = static_cast<uint32_t>(m_spotLights.size());

        commandList->SetGraphics32BitConstants(RootParameters::LightPropertiesCB, lightProps);
        commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::PointLights, m_pointLights);
        commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::SpotLights, m_spotLights);

        XMMATRIX translationMatrix = XMMatrixTranslation(0.f, 0.f, 0.0f);
        XMMATRIX rotationMatrix = XMMatrixIdentity();
        XMMATRIX scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
        XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
        XMMATRIX viewMatrix = m_camera.get_ViewMatrix();
        XMMATRIX viewProjectionMatrix = viewMatrix * m_camera.get_ProjectionMatrix();
        Mat matrices;

        for (const auto& pEntity : pEntities)
        {
            if (pEntity->IsEnabled())
            {
                worldMatrix = pEntity->GetTransform().LocalToWorldMatrixDX();

                for (const auto& pRenderComponent : pEntity->GetRenderComponents())
                {
                    if (pRenderComponent->IsEnabled())
                    {
                        const Mesh* pEntityMesh = pRenderComponent->GetRenderable();
                        const UIQuadData* pQuadData = pRenderComponent->Get2DRenderable();
                        if (pEntityMesh != nullptr)
                        {
                            viewMatrix = m_camera.get_ViewMatrix();
                            viewProjectionMatrix = viewMatrix * m_camera.get_ProjectionMatrix();

                            ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

                            commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

                            commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, Material::White);
                            if (pEntityMesh->GetTextureResource() != nullptr)
                            {
                                commandList->SetShaderResourceView(RootParameters::Textures, 0, pEntityMesh->GetTextureResource()->GetInternalTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                            }
                            else
                            {
                                commandList->SetShaderResourceView(RootParameters::Textures, 0, m_defaultTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                            }

                            pEntityMesh->Render(*commandList);
                        }

                        if (pQuadData != nullptr)
                        {
                            XMVECTOR cameraPos = XMVectorSet(0, 0, 0, 1);
                            XMVECTOR cameraTarget = XMVectorSet(0, 0, 1, 1);
                            XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

                            m_UICamera.set_LookAt(cameraPos, cameraTarget, cameraUp);
                            m_UICamera.set_Orthographic(static_cast<float>(a_rWindow.m_clientWidth), static_cast<float>(a_rWindow.m_clientHeight), 0.1f, 10000.0f);

                            commandList->SetPipelineState(m_UIPipelineState);
                            commandList->SetGraphicsRootSignature(m_UIRootSignature);

                            XMMATRIX testMatrix = pQuadData->GetWorldMatrix(static_cast<float>(a_rWindow.GetClientWidth()), static_cast<float>(a_rWindow.GetClientHeight()));

                            viewMatrix = m_UICamera.get_ViewMatrix();
                            viewProjectionMatrix = viewMatrix * m_UICamera.get_OrthographicMatrix();

                            ComputeMatrices(testMatrix, viewMatrix, viewProjectionMatrix, matrices);

                            commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
                            commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, Material::White);
                            if (pQuadData->GetTextureResource() != nullptr)
                            {
                                commandList->SetShaderResourceView(RootParameters::Textures, 0, pQuadData->GetTextureResource()->GetInternalTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                            }
                            else
                            {
                                commandList->SetShaderResourceView(RootParameters::Textures, 0, m_defaultTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                            }

                            m_UIQuadMesh->Render(*commandList);
                        }
                    }
                }


            }
        }
        for (const auto& collider : Helios::CollisionManager::GetInstance().GetColliders())
        {
            glm::vec3 position = collider->GetTransform().GetLocalPosition();
            glm::vec3 rotation = collider->GetTransform().GetLocalRotationInEuler();
            glm::vec3 scale = collider->GetTransform().GetScale();

            if (collider->IsEnabled() && collider->GetVisibility())
            {
                if (collider->GetDrawableShape() != nullptr)
                {
                    translationMatrix = XMMatrixTranslation((position.x + collider->GetOffset().x) * 0.05f,
                        -(position.y + collider->GetOffset().y) * 0.05f
                        , 0.0f);
                    rotationMatrix =
                        XMMatrixRotationX(Math::Radians(0)) *
                        XMMatrixRotationY(Math::Radians(0)) *
                        XMMatrixRotationZ(Math::Radians(rotation.z + collider->m_offsetRotation));

                    const float directScaling = (collider->m_useDirectScale ? 1.f : 0.05f);
                    scaleMatrix = XMMatrixScaling(collider->GetSize().x * directScaling,
                        collider->GetSize().y * directScaling,  0.1f);
                    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

                    if (collider->GetTransform().GetParent())
                    {
                        position = collider->GetTransform().GetParent()->GetLocalPosition();
                        rotation = collider->GetTransform().GetParent()->GetLocalRotationInEuler();
                        scale = collider->GetTransform().GetParent()->GetScale();

                        XMMATRIX parentMatrix;
                        translationMatrix = XMMatrixTranslation(position.x * 0.05f, -position.y * 0.05f, 0.0f);
                        rotationMatrix =
                            XMMatrixRotationX(Math::Radians(rotation.x)) *
                            XMMatrixRotationY(Math::Radians(rotation.y)) *
                            XMMatrixRotationZ(Math::Radians(rotation.z));
                        scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
                        parentMatrix = scaleMatrix * rotationMatrix * translationMatrix;
                        worldMatrix = worldMatrix * parentMatrix;
                    }

                    viewMatrix = m_camera.get_ViewMatrix();
                    viewProjectionMatrix = viewMatrix * m_camera.get_ProjectionMatrix();

                    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

                    commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

                    commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, Material::RedRubber);


                    commandList->SetShaderResourceView(RootParameters::Textures, 0, m_defaultTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                    collider->GetDrawableShape()->Render(*commandList);


                }
            }
        }

        commandList->SetShaderResourceView(RootParameters::Textures, 0, m_defaultTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // Perform HDR -> SDR tonemapping directly to the Window's render target.
        commandList->SetRenderTarget(a_rWindow.GetRenderTarget());
        commandList->SetViewport(a_rWindow.GetRenderTarget().GetViewport(
            DirectX::XMFLOAT2(viewportRanges.z, viewportRanges.w), DirectX::XMFLOAT2(viewportRanges.x, viewportRanges.y))
        );
        commandList->SetPipelineState(m_sDRPipelineState);
        commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->SetGraphicsRootSignature(m_sDRRootSignature);
        commandList->SetGraphics32BitConstants(0, g_TonemapParameters);
        commandList->SetShaderResourceView(1, 0, m_hDRRenderTarget.GetTexture(Color0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        commandList->Draw(3);

        commandQueue->ExecuteCommandList(commandList);

        // Present
        a_rWindow.Present();
    }
}

void Pipeline::RescaleHDRRenderTarget(float scale)
{
    uint32_t width = static_cast<uint32_t>(m_width * scale);
    uint32_t height = static_cast<uint32_t>(m_height * scale);

    width = std::clamp<uint32_t>(width, 1, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);
    height = std::clamp<uint32_t>(height, 1, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);

    m_hDRRenderTarget.Resize(width, height);
}

void Pipeline::OnResize(uint32_t a_width, uint32_t a_height)
{
    //TODO: refactor to use uint32_t and not int
    if (m_width != static_cast<int>(a_width) || m_height != static_cast<int>(a_height))
    {
        m_width = std::max(1, static_cast<int>(a_width));
        m_height = std::max(1, static_cast<int>(a_height));

        float fov = m_camera.get_FoV();
        float aspectRatio = m_width / (float)m_height;
        m_camera.set_Projection(fov, aspectRatio, 0.1f, 10000.0f);
        m_UICamera.set_Orthographic(static_cast<float>(m_width), static_cast<float>(m_height), 0.1f, 10000.0f);

        RescaleHDRRenderTarget(m_renderScale);
    }
}

Pipeline::Pipeline(uint32_t a_width, uint32_t a_height)
    : m_scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX))
    , m_forward(0)
    , m_backward(0)
    , m_left(0)
    , m_right(0)
    , m_up(0)
    , m_down(0)
    , m_pitch(0)
    , m_yaw(0)
    , m_animateLights(false)
    , m_shift(false)
    , m_width(0)
    , m_height(0)
    , m_renderScale(1.0f)
    , m_isContentLoaded(false)
{
    //TODO: remove below assignments, pipeline should not have a width and height (it should be fetched from the window)
    a_width = std::clamp<uint32_t>(a_width, 1, std::numeric_limits<uint32_t>::max());
    a_height = std::clamp<uint32_t>(a_height, 1, std::numeric_limits<uint32_t>::max());

    m_width = a_width;
    m_height = a_height;

    if (!DirectX::XMVerifyCPUSupport())
    {
        MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
        HE_CORE_ASSERT(false, "Failed to verify DirectX Math library support.");
    }

    /*XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
    XMVECTOR cameraTarget = XMVectorSet(0, 5, 0, 1);
    XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);*/

    XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
    XMVECTOR cameraTarget = XMVectorSet(0, 5, 0, 1);
    XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

    m_camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
    m_camera.set_Projection(45.0f, static_cast<float>(a_width) / static_cast<float>(a_height), 0.1f, 10000.0f);

    m_UICamera.set_LookAt(cameraPos, cameraTarget, cameraUp);
    m_UICamera.set_Orthographic(static_cast<float>(a_width), static_cast<float>(a_height), 0.1f, 10000.0f);

    m_pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

    if (m_pAlignedCameraData != nullptr)
    {
        m_pAlignedCameraData->m_initialCamPos = m_camera.get_Translation();
        m_pAlignedCameraData->m_initialCamRot = m_camera.get_Rotation();
        m_pAlignedCameraData->m_initialFov = m_camera.get_FoV();
    }

    LoadContent(a_width, a_height);
}

Pipeline::~Pipeline()
{
    _aligned_free(m_pAlignedCameraData);
}

//TODO: move this to RenderHelpers
std::string ExecutablePath()
{
    wchar_t buffer[MAX_PATH];

    GetModuleFileName(NULL, buffer, MAX_PATH);

    char s[MAX_PATH];
    sprintf(s, "%ls", buffer);

    std::string temp = std::string(s);

    const std::string exeName = "GradiusVI.exe";
    const size_t pos = temp.find(exeName);

    if (pos != std::string::npos)
    {
        // If found then erase it from string
        temp.erase(pos, exeName.length());
    }

    return temp;
}

//TODO: move this to RenderHelpers
std::wstring ExecutablePathW()
{
    std::string executablePathString = ExecutablePath();
    std::wstring executablePathWString(executablePathString.length(), L' '); // Make room for characters
    std::copy(executablePathString.begin(), executablePathString.end(), executablePathWString.begin());

    return executablePathWString;
}

void Pipeline::LoadContent(uint32_t a_width, uint32_t a_height)
{
    auto device = DX12Renderer::GetInstance().GetDevice();
    auto commandQueue = DX12Renderer::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    auto commandList = commandQueue->GetCommandList();

    // Create a Cube mesh
    m_cubeMesh = Mesh::CreateCube();
    m_sphereMesh = Mesh::CreateSphere();
    m_coneMesh = Mesh::CreateCone();
    m_torusMesh = Mesh::CreateTorus();
    m_planeMesh = Mesh::CreatePlane();
    // Create an inverted (reverse winding order) cube so the insides are not clipped.
    m_skyboxMesh = Mesh::CreateCube(1.0f, true);

    m_UIQuadMesh = Mesh::CreatePlane(1.0f, 1.0f);

    // Load some textures
    commandList->LoadTextureFromFile(m_defaultTexture, L"Assets/Textures/DefaultWhite.bmp");
    commandList->LoadTextureFromFile(m_spaceTexture, L"Assets/Textures/space.png");

        // Create a cubemap for the HDR panorama.
    auto cubemapDesc = m_spaceTexture.GetD3D12ResourceDesc();
    cubemapDesc.Width = cubemapDesc.Height = 1024;
    cubemapDesc.DepthOrArraySize = 6;
    cubemapDesc.MipLevels = 0;

    m_spaceCubemap = Texture(cubemapDesc, nullptr, TextureUsage::Albedo, L"Space Cubemap");
    // Convert the 2D panorama to a 3D cubemap.
    commandList->PanoToCubemap(m_spaceCubemap, m_spaceTexture);

    // Create an HDR intermediate render target.
    DXGI_FORMAT HDRFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

    // Create an off-screen render target with a single color buffer and a depth buffer.
    auto colorDesc = CD3DX12_RESOURCE_DESC::Tex2D(HDRFormat, a_width, a_height);
    colorDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE colorClearValue;
    colorClearValue.Format = colorDesc.Format;
    colorClearValue.Color[0] = 0.4f;
    colorClearValue.Color[1] = 0.6f;
    colorClearValue.Color[2] = 0.9f;
    colorClearValue.Color[3] = 1.0f;

    Texture HDRTexture = Texture(colorDesc, &colorClearValue,
        TextureUsage::RenderTarget,
        L"HDR Texture");

    // Create a depth buffer for the HDR render target.
    auto depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthBufferFormat, a_width, a_height);
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthClearValue;
    depthClearValue.Format = depthDesc.Format;
    depthClearValue.DepthStencil = { 1.0f, 0 };

    Texture depthTexture = Texture(depthDesc, &depthClearValue,
        TextureUsage::Depth,
        L"Depth Render Target");

    // Attach the HDR texture to the HDR render target.
    m_hDRRenderTarget.AttachTexture(AttachmentPoint::Color0, HDRTexture);
    m_hDRRenderTarget.AttachTexture(AttachmentPoint::DepthStencil, depthTexture);

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    // Create a root signature and PSO for the skybox shaders.
    {
        // Load the Skybox shaders.
        ComPtr<ID3DBlob> vs;
        ComPtr<ID3DBlob> ps;
        ThrowIfFailed(D3DReadFileToBlob((ExecutablePathW() + L"Assets\\Shaders\\Skybox_vs.cso").c_str(), &vs));
        ThrowIfFailed(D3DReadFileToBlob((ExecutablePathW() + L"Assets\\Shaders\\Skybox_ps.cso").c_str(), &ps));

        // Setup the input layout for the skybox vertex shader.
        D3D12_INPUT_ELEMENT_DESC inputLayout[1] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        // Allow input layout and deny unnecessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER1 rootParameters[2];
        rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
        rootSignatureDescription.Init_1_1(2, rootParameters, 1, &linearClampSampler, rootSignatureFlags);

        m_skyboxSignature.SetRootSignatureDesc(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);

        // Setup the Skybox pipeline state.
        struct SkyboxPipelineState
        {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
            CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
            CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
            CD3DX12_PIPELINE_STATE_STREAM_VS VS;
            CD3DX12_PIPELINE_STATE_STREAM_PS PS;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
            CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
        } skyboxPipelineStateStream;

        skyboxPipelineStateStream.pRootSignature = m_skyboxSignature.GetRootSignature().Get();
        skyboxPipelineStateStream.InputLayout = { inputLayout, 1 };
        skyboxPipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        skyboxPipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vs.Get());
        skyboxPipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(ps.Get());
        skyboxPipelineStateStream.RTVFormats = m_hDRRenderTarget.GetRenderTargetFormats();
        CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        skyboxPipelineStateStream.BlendDesc = blendDesc;

        D3D12_PIPELINE_STATE_STREAM_DESC skyboxPipelineStateStreamDesc = {
            sizeof(SkyboxPipelineState), &skyboxPipelineStateStream
        };
        ThrowIfFailed(device->CreatePipelineState(&skyboxPipelineStateStreamDesc, IID_PPV_ARGS(&m_skyboxPipelineState)));
    }

    // Create a root signature for the HDR pipeline.
    {
        // Load the HDR shaders.
        ComPtr<ID3DBlob> vs;
        ComPtr<ID3DBlob> ps;
        ThrowIfFailed(D3DReadFileToBlob((ExecutablePathW() + L"Assets\\Shaders\\HDR_vs.cso").c_str(), &vs));
        ThrowIfFailed(D3DReadFileToBlob((ExecutablePathW() + L"Assets\\Shaders\\HDR_ps.cso").c_str(), &ps));

        // Allow input layout and deny unnecessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

        CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters];
        rootParameters[RootParameters::MatricesCB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[RootParameters::MaterialCB].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameters::LightPropertiesCB].InitAsConstants(sizeof(LightProperties) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameters::PointLights].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameters::SpotLights].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameters::Textures].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
        CD3DX12_STATIC_SAMPLER_DESC anisotropicSampler(0, D3D12_FILTER_ANISOTROPIC);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
        rootSignatureDescription.Init_1_1(RootParameters::NumRootParameters, rootParameters, 1, &linearRepeatSampler, rootSignatureFlags);

        m_hDRRootSignature.SetRootSignatureDesc(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);

        // Setup the HDR pipeline state.
        struct HDRPipelineStateStream
        {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
            CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
            CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
            CD3DX12_PIPELINE_STATE_STREAM_VS VS;
            CD3DX12_PIPELINE_STATE_STREAM_PS PS;
            CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
            CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
        } hdrPipelineStateStream;

        hdrPipelineStateStream.pRootSignature = m_hDRRootSignature.GetRootSignature().Get();
        hdrPipelineStateStream.InputLayout = { VertexPositionNormalTexture::InputElements, VertexPositionNormalTexture::InputElementCount };
        hdrPipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        hdrPipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vs.Get());
        hdrPipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(ps.Get());
        hdrPipelineStateStream.DSVFormat = m_hDRRenderTarget.GetDepthStencilFormat();
        hdrPipelineStateStream.RTVFormats = m_hDRRenderTarget.GetRenderTargetFormats();
        CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        hdrPipelineStateStream.BlendDesc = blendDesc;

        D3D12_PIPELINE_STATE_STREAM_DESC hdrPipelineStateStreamDesc = {
            sizeof(HDRPipelineStateStream), &hdrPipelineStateStream
        };
        ThrowIfFailed(device->CreatePipelineState(&hdrPipelineStateStreamDesc, IID_PPV_ARGS(&m_hDRPipelineState)));
    }

    // Create the SDR Root Signature
    {
        CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER1 rootParameters[2];
        rootParameters[0].InitAsConstants(sizeof(TonemapParameters) / 4, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_STATIC_SAMPLER_DESC linearClampsSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
        rootSignatureDescription.Init_1_1(2, rootParameters, 1, &linearClampsSampler);

        m_sDRRootSignature.SetRootSignatureDesc(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);

        // Create the SDR PSO
        ComPtr<ID3DBlob> vs;
        ComPtr<ID3DBlob> ps;
        ThrowIfFailed(D3DReadFileToBlob((ExecutablePathW() + L"Assets\\Shaders\\HDRtoSDR_vs.cso").c_str(), &vs));
        ThrowIfFailed(D3DReadFileToBlob((ExecutablePathW() + L"Assets\\Shaders\\HDRtoSDR_ps.cso").c_str(), &ps));

        CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
        rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

        struct SDRPipelineStateStream
        {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
            CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
            CD3DX12_PIPELINE_STATE_STREAM_VS VS;
            CD3DX12_PIPELINE_STATE_STREAM_PS PS;
            CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
        } sdrPipelineStateStream;

        sdrPipelineStateStream.pRootSignature = m_sDRRootSignature.GetRootSignature().Get();
        sdrPipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        sdrPipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vs.Get());
        sdrPipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(ps.Get());
        sdrPipelineStateStream.Rasterizer = rasterizerDesc;
        CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);

        //TODO: refactor, window should not be accessed this way
        std::shared_ptr<Window> pWindow = DX12Renderer::GetInstance().GetFirstWindow();
        sdrPipelineStateStream.RTVFormats = pWindow->GetRenderTarget().GetRenderTargetFormats();

        D3D12_PIPELINE_STATE_STREAM_DESC sdrPipelineStateStreamDesc = {
            sizeof(SDRPipelineStateStream), &sdrPipelineStateStream
        };
        ThrowIfFailed(device->CreatePipelineState(&sdrPipelineStateStreamDesc, IID_PPV_ARGS(&m_sDRPipelineState)));
    }

    // Create the UI Root Signature
    {
        // Load the HDR shaders.
        ComPtr<ID3DBlob> vs;
        ComPtr<ID3DBlob> ps;
        ThrowIfFailed(D3DReadFileToBlob((ExecutablePathW() + L"Assets\\Shaders\\Sprite_vs.cso").c_str(), &vs));
        ThrowIfFailed(D3DReadFileToBlob((ExecutablePathW() + L"Assets\\Shaders\\Sprite_ps.cso").c_str(), &ps));

        // Allow input layout and deny unnecessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

        CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters];
        rootParameters[RootParameters::MatricesCB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[RootParameters::MaterialCB].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameters::LightPropertiesCB].InitAsConstants(sizeof(LightProperties) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameters::PointLights].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameters::SpotLights].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameters::Textures].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
        CD3DX12_STATIC_SAMPLER_DESC anisotropicSampler(0, D3D12_FILTER_ANISOTROPIC);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
        rootSignatureDescription.Init_1_1(RootParameters::NumRootParameters, rootParameters, 1, &linearRepeatSampler, rootSignatureFlags);

        m_UIRootSignature.SetRootSignatureDesc(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);

        // Setup the UI pipeline state.
        struct UIPipelineStateStream
        {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
            CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
            CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
            CD3DX12_PIPELINE_STATE_STREAM_VS VS;
            CD3DX12_PIPELINE_STATE_STREAM_PS PS;
            CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
            CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
        } uiPipelineStateStream;

        uiPipelineStateStream.pRootSignature = m_UIRootSignature.GetRootSignature().Get();
        uiPipelineStateStream.InputLayout = { VertexPositionNormalTexture::InputElements, VertexPositionNormalTexture::InputElementCount };
        uiPipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        uiPipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vs.Get());
        uiPipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(ps.Get());
        uiPipelineStateStream.DSVFormat = m_hDRRenderTarget.GetDepthStencilFormat();
        uiPipelineStateStream.RTVFormats = m_hDRRenderTarget.GetRenderTargetFormats();
        CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        uiPipelineStateStream.BlendDesc = blendDesc;

        D3D12_PIPELINE_STATE_STREAM_DESC hdrPipelineStateStreamDesc = {
            sizeof(UIPipelineStateStream), &uiPipelineStateStream
        };
        ThrowIfFailed(device->CreatePipelineState(&hdrPipelineStateStreamDesc, IID_PPV_ARGS(&m_UIPipelineState)));
    }

    //load test gltf model
    {
        // m_gltfTestMesh.LoadFromGLTF("Assets\\GLTF\\Duck\\Duck.gltf", 0, m_gltfTestMaterial);
    }

    auto fenceValue = commandQueue->ExecuteCommandList(commandList);
    commandQueue->WaitForFenceValue(fenceValue);

    m_isContentLoaded = true;
}
