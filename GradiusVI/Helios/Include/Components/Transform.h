#pragma once
#include "Core/EntityComponent/Component.h"
#include "DirectXMath.h"

namespace Helios
{
    class Transform : public Component
    {
        RTTR_ENABLE(Component)
            friend class Deserializer;
    public:
        //Transform(Entity& a_entity);
        Transform(Entity& a_owner, const ComponentID& a_id);

        int GetChildrenCount() const
        {
            return static_cast<int>(m_children.size());
        }

        void AddChild(Transform* a_child);
        void RemoveChild(const Transform& a_child);
        void SetParent(Transform* a_parent);
        void SetParent(const EntityID& a_ownerId);

        void SetPosition(Vec3 a_pos);
        void SetPosition(float a_x, float a_y, float a_z) { SetPosition({ a_x, a_y, a_z }); };
        void SetPosition(float a_x, float a_y) { SetPosition({ a_x, a_y, 0.f }); };
        Vec3 GetPosition();

        const Vec3& GetLocalPosition() const { return m_localPosition; };
        void SetLocalPosition(const Vec3& a_localPos)
        {
            const Vec3 oldPos = m_localPosition;
            m_localPosition = a_localPos;

            if (m_localPosition != oldPos)
            {
                SetDirty();
            }
        };

        Quaternion GetRotation() const;
        Vec3 GetRotationInEuler() const;
        void SetRotation(Quaternion a_rotation);
        void SetRotation(float a_angle, const Vec3& a_axis);
        void SetRotation(const Vec3& a_orientation) { SetRotation(Quaternion(glm::radians(a_orientation))); }
        void SetRotation(float a_x, float a_y, float a_z) { SetRotation({ a_x, a_y, a_z }); };
        void Rotate(float a_angle, const Vec3& a_axis);

        const Quaternion& GetLocalRotation() const { return m_localRotation; }
        Vec3 GetLocalRotationInEuler() const { return glm::degrees(glm::eulerAngles(m_localRotation)); }
        void SetLocalRotation(const Quaternion& a_rotation)
        {
            const Quaternion oldRot = m_localRotation;
            m_localRotation = a_rotation;

            if(m_localRotation != oldRot)
            {
                SetDirty();
            }
        }
        void SetLocalRotation(float a_angle, const Vec3& a_axis) { SetLocalRotation(Quaternion(glm::radians(a_angle), a_axis)); }
        void SetLocalRotation(const Vec3& a_orientation) { SetLocalRotation(Quaternion(glm::radians(a_orientation))); }
        void SetLocalRotation(float a_x, float a_y, float a_z) { SetLocalRotation({ a_x, a_y, a_z }); }

        const Vec3& GetScale() const { return m_localScale; }
        void SetScale(const Vec3& a_scale) { m_localScale = a_scale; }
        void SetScale(float a_x, float a_y, float a_z) { SetScale({ a_x, a_y, a_z }); }
        void SetScale(float a_uniform) { SetScale({ a_uniform, a_uniform, a_uniform }); }

        void ResetParent();

        Transform* GetParent() const { return m_parent; }
        Transform* GetRoot() const { return m_root; }

        const std::vector<Transform*>& GetChildren() const { return m_children; }

        Mat4& LocalToWorldMatrix();
        Mat4& WorldToLocalMatrix();
        DirectX::XMMATRIX& LocalToWorldMatrixDX();
        DirectX::XMMATRIX& WorldToLocalMatrixDX();

        void SetDirty();

        Mat4 m_localToWorldMatrix;
    private:
        Vec3 m_localPosition;
        Quaternion m_localRotation;
        Vec3 m_localScale;

        void ClearChildren();

        void RecalculateRoot();

        std::vector<Transform*> m_children;

        Transform* m_parent;
        int m_parentOwnerId;

        Transform* m_root;

        Mat4 m_modelMatrix;
        Mat4 m_inverseModelMatrix;

        Mat4 m_worldToLocalMatrix;

        DirectX::XMMATRIX m_modelDXMatrix;
        DirectX::XMMATRIX m_inverseModelDXMatrix;
        DirectX::XMMATRIX m_localToWorldMatrixDX;
        DirectX::XMMATRIX m_worldToLocalMatrixDX;

        bool m_isDirty;
        bool m_isDXDirty;
        bool m_isInverseDirty;
        bool m_isInverseDXDirty;

        Mat4& CalculateModelMatrixColumn();
        DirectX::XMMATRIX& CalculateModelMatrixDX();
        bool CanBeParent(const Transform& a_parent) const;

        RTTR_REGISTRATION_FRIEND
    };
}

