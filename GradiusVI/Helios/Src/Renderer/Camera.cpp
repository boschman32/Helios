#include "hepch.h"
#include "Renderer/Camera.h"

#include <cassert>

using namespace DirectX;

Camera::Camera()
    : m_ViewDirty(true)
    , m_InverseViewDirty(true)
    , m_ProjectionDirty(true)
    , m_InverseProjectionDirty(true)
    , m_OrthographicDirty(true)
    , m_InverseOrthographicDirty(true)
    , m_vFoV(45.0f)
    , m_AspectRatio(1.0f)
    , m_zPerspectiveNear(0.1f)
    , m_zPerspectiveFar(100.0f)
    , m_width(1.f)
    , m_height(1.f)
    , m_zOrthographicNear(0.1f)
    , m_zOrthographicFar(100.f)
{
    pData = (AlignedData*)_aligned_malloc(sizeof(AlignedData), 16);
    if (pData != nullptr)
    {
        pData->m_Translation = XMVectorZero();
        pData->m_Rotation = XMQuaternionIdentity();
    }
    else
    {
        assert(false);
    }
}

Camera::~Camera()
{
    _aligned_free(pData);
}

void XM_CALLCONV Camera::set_LookAt(FXMVECTOR eye, FXMVECTOR target, FXMVECTOR up)
{
    pData->m_ViewMatrix = XMMatrixLookAtLH(eye, target, up);

    pData->m_Translation = eye;
    pData->m_Rotation = XMQuaternionRotationMatrix(XMMatrixTranspose(pData->m_ViewMatrix));

    m_InverseViewDirty = true;
    m_ViewDirty = false;
}

XMMATRIX Camera::get_ViewMatrix() const
{
    if (m_ViewDirty)
    {
        UpdateViewMatrix();
    }
    return pData->m_ViewMatrix;
}

XMMATRIX Camera::get_InverseViewMatrix() const
{
    if (m_InverseViewDirty)
    {
        pData->m_InverseViewMatrix = XMMatrixInverse(nullptr, pData->m_ViewMatrix);
        m_InverseViewDirty = false;
    }

    return pData->m_InverseViewMatrix;
}

void Camera::set_Projection(float fovy, float aspect, float zNear, float zFar)
{
    m_vFoV = fovy;
    m_AspectRatio = aspect;
    m_zPerspectiveNear = zNear;
    m_zPerspectiveFar = zFar;

    m_ProjectionDirty = true;
    m_InverseProjectionDirty = true;
}

XMMATRIX Camera::get_ProjectionMatrix() const
{
    if (m_ProjectionDirty)
    {
        UpdateProjectionMatrix();
    }

    return pData->m_ProjectionMatrix;
}

XMMATRIX Camera::get_InverseProjectionMatrix() const
{
    if (m_InverseProjectionDirty)
    {
        UpdateInverseProjectionMatrix();
    }

    return pData->m_InverseProjectionMatrix;
}

void Camera::set_Orthographic(float width, float height, float zNear, float zFar)
{
    m_width = width;
    m_height = height;
    m_zOrthographicNear = zNear;
    m_zOrthographicFar = zFar;

    m_OrthographicDirty = true;
    m_InverseOrthographicDirty = true;
}

DirectX::XMMATRIX Camera::get_OrthographicMatrix() const
{
    if (m_OrthographicDirty)
    {
        UpdateOrthographicMatrix();
    }

    return pData->m_OrthographicMatrix;
}

DirectX::XMMATRIX Camera::get_InverseOrthographicMatrix() const
{
    if (m_InverseOrthographicDirty)
    {
        UpdateInverseOrthographicMatrix();
    }

    return pData->m_InverseOrthographicMatrix;
}

void Camera::set_FoV(float fovy)
{
    if (m_vFoV != fovy)
    {
        m_vFoV = fovy;
        m_ProjectionDirty = true;
        m_InverseProjectionDirty = true;
        m_OrthographicDirty = true;
        m_InverseOrthographicDirty = true;
    }
}

float Camera::get_FoV() const
{
    return m_vFoV;
}

void XM_CALLCONV Camera::set_Translation(FXMVECTOR translation)
{
    pData->m_Translation = translation;
    m_ViewDirty = true;
}

XMVECTOR Camera::get_Translation() const
{
    return pData->m_Translation;
}

void Camera::set_Rotation(FXMVECTOR rotation)
{
    pData->m_Rotation = rotation;
}

XMVECTOR Camera::get_Rotation() const
{
    return pData->m_Rotation;
}

void XM_CALLCONV Camera::Translate(FXMVECTOR translation, Space space)
{
    switch (space)
    {
    case Space::Local:
    {
        pData->m_Translation += XMVector3Rotate(translation, pData->m_Rotation);
    }
    break;
    case Space::World:
    {
        pData->m_Translation += translation;
    }
    break;
    }

    pData->m_Translation = XMVectorSetW(pData->m_Translation, 1.0f);

    m_ViewDirty = true;
    m_InverseViewDirty = true;
}

void Camera::Rotate(FXMVECTOR quaternion)
{
    pData->m_Rotation = XMQuaternionMultiply(pData->m_Rotation, quaternion);

    m_ViewDirty = true;
    m_InverseViewDirty = true;
}

void Camera::UpdateViewMatrix() const
{
    XMMATRIX rotationMatrix = XMMatrixTranspose(XMMatrixRotationQuaternion(pData->m_Rotation));
    XMMATRIX translationMatrix = XMMatrixTranslationFromVector(-(pData->m_Translation));

    pData->m_ViewMatrix = translationMatrix * rotationMatrix;

    m_InverseViewDirty = true;
    m_ViewDirty = false;
}

void Camera::UpdateInverseViewMatrix() const
{
    if (m_ViewDirty)
    {
        UpdateViewMatrix();
    }

    pData->m_InverseViewMatrix = XMMatrixInverse(nullptr, pData->m_ViewMatrix);
    m_InverseViewDirty = false;
}

void Camera::UpdateProjectionMatrix() const
{
    pData->m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_vFoV), m_AspectRatio, m_zPerspectiveNear, m_zPerspectiveFar);

    m_ProjectionDirty = false;
    m_InverseProjectionDirty = true;
}

void Camera::UpdateInverseProjectionMatrix() const
{
    if (m_ProjectionDirty)
    {
        UpdateProjectionMatrix();
    }

    pData->m_InverseProjectionMatrix = XMMatrixInverse(nullptr, pData->m_ProjectionMatrix);
    m_InverseProjectionDirty = false;
}

void Camera::UpdateOrthographicMatrix() const
{
    /*pData->m_OrthographicMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_vFoV), m_AspectRatio, m_zPerspectiveNear, m_zPerspectiveFar);*/
    pData->m_OrthographicMatrix = XMMatrixOrthographicLH(m_width, m_height, m_zOrthographicNear, m_zOrthographicFar);

    m_OrthographicDirty = false;
    m_InverseOrthographicDirty = true;
}

void Camera::UpdateInverseOrthographicMatrix() const
{
    if (m_OrthographicDirty)
    {
        UpdateOrthographicMatrix();
    }

    pData->m_InverseOrthographicMatrix = XMMatrixInverse(nullptr, pData->m_OrthographicMatrix);
    m_InverseOrthographicDirty = false;
}
