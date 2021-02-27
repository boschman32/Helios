#pragma once
#include "Components/ScriptableComponent.h"

namespace Helios
{
    class QuadCollider;
}

namespace Gradius
{      
    enum class EBulletType
    {
        None,
        DefaultBullet,
        Missile,
        Double,
        Laser,
        BigCore
    };

    class BaseBullet : public Helios::ScriptableComponent
    {
        RTTR_ENABLE(Helios::ScriptableComponent)
    public:
        BaseBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id);
        virtual ~BaseBullet();

        std::function<void(BaseBullet&)> m_onDestroyCallback;

        virtual EBulletType GetBulletType() const { return EBulletType::None; };
    protected:
        virtual void Move(float) { };

        int m_damage                    { 1 };
        float m_bulletSpeed             { 0.f };
        glm::vec2 m_direction           { 1.f, 0.f };

        Helios::QuadCollider* m_collider = nullptr;
        RTTR_REGISTRATION_FRIEND;
    };
}
