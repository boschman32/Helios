#include "gvipch.h"
#include "Collider/ColliderTags.h"
#include "Ships/BaseShip/BaseShip.h"
#include "Bullets/DoubleBullet.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/Colliders/QuadCollider.h"

namespace Gradius
{
    DoubleBullet::DoubleBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id)
        : BaseBullet(a_owner, a_id)
    {
        
    }

    void DoubleBullet::Start()
    {
        m_collider = m_owner->GetComponent<Helios::QuadCollider>();

        GetTransform().SetRotation(45.f, Vec3(0.f, 0.f, 1.f));
        m_collider->m_offsetRotation = -4.5;
        m_collider->SetTag(Tags::m_bulletPlayerTag);
    }

    void DoubleBullet::OnOverlapBegin(Helios::Collider* a_collider)
    {
        if (a_collider->GetName() == Tags::m_enemyTag)
        {
            a_collider->GetOwner().GetComponent<BaseShip>()->DamageShip(m_damage);
            Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
        }
    }

    void DoubleBullet::Update(float deltaTime)
    {
        Move(deltaTime);
        CheckBounds();
    }

    void DoubleBullet::Move(float deltaTime)
    {
    	Vec3 pos = GetTransform().GetPosition();
    	pos.x += m_direction.x * (m_bulletSpeed * deltaTime);
    	pos.y += m_direction.y * (m_bulletSpeed * deltaTime);
    	GetOwner().GetTransform().SetPosition(pos);
    }

    void DoubleBullet::CheckBounds()
    {
        const Vec2& bulletPos = GetOwner().GetComponent<Helios::Transform>()->GetPosition();
        if (m_direction.x >= 0)
        {
            if (bulletPos.x > 1500 || bulletPos.y < -100)
            {
                Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
            }
        }
    }
}