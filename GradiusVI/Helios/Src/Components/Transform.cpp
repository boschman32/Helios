#include "hepch.h"
#include "Components/Transform.h"
#include "Core/EntityComponent/EntityManager.h"

namespace Helios
{
    Transform::Transform(Entity& a_owner, const ComponentID& a_id)
        : Component(a_owner, a_id),
        m_localToWorldMatrix(1.f),
        m_localPosition(0.f),
        m_localRotation(1.f, 0.f, 0.f, 0.f),
        m_localScale(1.0f),
        m_parent(nullptr),
        m_parentOwnerId(-1),
        m_root(this),
        m_modelMatrix(1.f),
        m_inverseModelMatrix(1.f),
        m_worldToLocalMatrix(1.f),
        m_modelDXMatrix(DirectX::XMMatrixIdentity()),
        m_inverseModelDXMatrix(DirectX::XMMatrixIdentity()),
        m_localToWorldMatrixDX(DirectX::XMMatrixIdentity()),
        m_worldToLocalMatrixDX(DirectX::XMMatrixIdentity()),
        m_isDirty(false),
        m_isInverseDirty(false),
        m_isDXDirty(false),
        m_isInverseDXDirty(false)
    {
        SetDirty();
    }

    void Transform::SetPosition(Vec3 a_pos)
    {
        const Vec3 oldPos = m_localPosition;
        if (m_parent != nullptr)
        {
            m_localPosition = WorldToLocalMatrix() * Vec4(a_pos, 1.f);
        }
        else
        {
            m_localPosition = a_pos;
        }

        //if (m_localPosition != oldPos)
        {
            SetDirty();
        }
    }

    Vec3 Transform::GetPosition()
    {
        if (m_parent != nullptr)
        {
            Mat4& worldMatrix = LocalToWorldMatrix();
            return { worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2] };
        }
        return m_localPosition;
    }

    void Transform::SetRotation(Quaternion a_rotation)
    {
        if (m_localRotation != a_rotation)
        {
            SetDirty();
        }
        m_localRotation = a_rotation;
    }

    Quaternion Transform::GetRotation() const
    {
        Quaternion parentRotation(1.f, 0.f, 0.f, 0.f);
        if (m_parent != nullptr)
        {
            parentRotation = m_parent->GetRotation();
        }

        return parentRotation * m_localRotation;
    }

    void Transform::SetRotation(float a_angle, const Vec3& a_axis)
    {
        const Quaternion rotX = glm::angleAxis(glm::radians(a_angle), Vec3{ a_axis.x, 0.f, 0.f });
        const Quaternion rotY = glm::angleAxis(glm::radians(a_angle), Vec3{ 0.f, a_axis.y, 0.f });
        const Quaternion rotZ = glm::angleAxis(glm::radians(a_angle), Vec3{ 0.f, 0.f, a_axis.z });

        SetRotation(rotX * rotY * rotZ);
    }

    void Transform::Rotate(float a_angle, const Vec3& a_axis)
    {
        const Quaternion rotX = glm::angleAxis(glm::radians(a_angle), Vec3{ a_axis.x, 0.f, 0.f });
        const Quaternion rotY = glm::angleAxis(glm::radians(a_angle), Vec3{ 0.f, a_axis.y, 0.f });
        const Quaternion rotZ = glm::angleAxis(glm::radians(a_angle), Vec3{ 0.f, 0.f, a_axis.z });

        SetRotation((rotX * rotY * rotZ) * m_localRotation);
    }

    Vec3 Transform::GetRotationInEuler() const
    {
        return glm::degrees(glm::eulerAngles(GetRotation()));
    }

    void Transform::SetDirty()
    {
        if (!m_isDirty || !m_isDXDirty)
        {
            m_isDirty = true;
            m_isInverseDirty = true;
            m_isDXDirty = true;
            m_isInverseDXDirty = true;

            for (auto c : m_children)
            {
                c->SetDirty();
            }
        }
    }

    Mat4& Transform::WorldToLocalMatrix()
    {
        if (m_isInverseDirty)
        {
            m_inverseModelMatrix = glm::inverse(LocalToWorldMatrix());

            m_isInverseDirty = false;
        }
        return m_inverseModelMatrix;
    }

    DirectX::XMMATRIX& Transform::WorldToLocalMatrixDX()
    {
        if (m_isInverseDXDirty)
        {
            DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(LocalToWorldMatrixDX());
            m_inverseModelDXMatrix = DirectX::XMMatrixInverse(&det, m_modelDXMatrix);

            m_isInverseDXDirty = false;
        }
        return m_inverseModelDXMatrix;
    }

    Mat4& Transform::LocalToWorldMatrix()
    {
        if (m_isDirty)
        {
            m_localToWorldMatrix = CalculateModelMatrixColumn();

            if (m_parent != nullptr)
            {
                m_localToWorldMatrix = m_parent->LocalToWorldMatrix() * m_localToWorldMatrix;
            }

            m_isDirty = false;
        }
        return m_localToWorldMatrix;
    }

    DirectX::XMMATRIX& Transform::LocalToWorldMatrixDX()
    {
        if (m_isDXDirty)
        {
            m_localToWorldMatrixDX = CalculateModelMatrixDX();
            if (m_parent != nullptr)
            {
                m_localToWorldMatrixDX = m_localToWorldMatrixDX * m_parent->LocalToWorldMatrixDX();
            }

            m_isDXDirty = false;
        }

        return m_localToWorldMatrixDX;
    }

    Mat4& Transform::CalculateModelMatrixColumn()
    {
        const glm::mat4 translation = glm::translate(glm::mat4(1.f), m_localPosition);
        const glm::mat4 scale = glm::scale(glm::mat4(1.f), m_localScale);
        const glm::mat4 rotation = glm::toMat4(m_localRotation);

        m_modelMatrix = translation * rotation * scale;
        return m_modelMatrix;
    }

    DirectX::XMMATRIX& Transform::CalculateModelMatrixDX()
    {
        const DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_localPosition.x * 0.05f, -m_localPosition.y * 0.05f, 0.0f);
        const Vec3 euler = GetLocalRotationInEuler();
        const DirectX::XMMATRIX rotationMatrix =
            DirectX::XMMatrixRotationX(glm::radians(euler.x)) *
            DirectX::XMMatrixRotationY(glm::radians(euler.y)) *
            DirectX::XMMatrixRotationZ(glm::radians(euler.z));
        const DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_localScale.x, m_localScale.y, m_localScale.z);

        m_modelDXMatrix = scaleMatrix * rotationMatrix * translationMatrix;

        return m_modelDXMatrix;
    }

    void Transform::AddChild(Transform* a_child)
    {
        const auto it = std::find(m_children.begin(), m_children.end(), a_child);
        if (it == m_children.end())
        {
            m_children.push_back(a_child);
        }
    }

    void Transform::RemoveChild(const Transform& a_child)
    {
        const auto it = std::find(m_children.begin(), m_children.end(), &a_child);
        if (it != m_children.end())
        {
            m_children.erase(it);
        }
    }

    void Transform::SetParent(Transform* a_parent)
    {
        if (a_parent != nullptr && CanBeParent(*a_parent))
        {
            //We already have a parent! De-child from the old parent
            if (m_parent != nullptr)
            {
                m_parent->RemoveChild(*this);
            }

            //If our parent has no parent this means that is the root of our hierarchy.
            if (a_parent->GetParent() == nullptr)
            {
                m_root = a_parent;
            }
            //Otherwise we ask our parent for the root.
            else
            {
                m_root = a_parent->GetRoot();
            }
            //Tell any children we have that our root has changed.
            RecalculateRoot();

            m_parent = a_parent;
            m_parentOwnerId = int(a_parent->GetOwner().GetID().Value());
            a_parent->AddChild(this);
        }
        else
        {
            ResetParent();
        }
    }

    void Transform::SetParent(const EntityID& a_ownerId)
    {
        Entity* owner = EntityManager::GetInstance().GetEntityByUniqueId(a_ownerId);
        if (owner != nullptr)
        {
            SetParent(&owner->GetTransform());
        }
    }

    void Transform::ResetParent()
    {
        if (m_parent != nullptr)
        {
            m_parent->RemoveChild(*this);
            m_parent = nullptr;
            m_root = this;
            m_parentOwnerId = -1;

            RecalculateRoot();
        }
    }

    bool Transform::CanBeParent(const Transform& a_parent) const
    {
        if (a_parent.GetParent() == nullptr)
        {
            return true;
        }
        //We are the root of this parent so we can't make this our parent.
        if (a_parent.GetRoot() == this)
        {
            return false;
        }

        //We are the parent of this child so we can't make this our parent.
        if (a_parent.GetParent() == this)
        {
            return false;
        }
        return true;
    }

    void Transform::RecalculateRoot()
    {
        for (auto child : m_children)
        {
            child->m_root = m_root;
            child->RecalculateRoot();
        }
    }

    void Transform::ClearChildren()
    {
        m_children.clear();
    }
}
