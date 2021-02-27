#include "gvipch.h"
#include "Collider/ColliderTags.h"
#include "Ships/BaseShip/BaseShip.h"
#include "Bullets/DefaultBullet.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/Colliders/QuadCollider.h"

namespace Gradius
{
    DefaultBullet::DefaultBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id)
        : BaseBullet(a_owner, a_id)
    {
        
    }

    void DefaultBullet::Start()
    {
        m_collider = m_owner->GetComponent<Helios::QuadCollider>();
        m_collider->SetTag(Tags::m_bulletPlayerTag);
    }

    void DefaultBullet::Update(float deltaTime)
    {
        Move(deltaTime);
        CheckBounds();
    }

    void DefaultBullet::OnOverlapBegin(Helios::Collider* a_collider)
    {
        if (a_collider->GetName() == Tags::m_enemyTag)
        {
            a_collider->GetOwner().GetComponent<BaseShip>()->DamageShip(m_damage);
            Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
        }
    }

    void DefaultBullet::Move(float deltaTime)
    {
    	Vec3 pos = GetTransform().GetPosition();
    	pos.x += m_direction.x * (m_bulletSpeed * deltaTime);
    	pos.y += m_direction.y * (m_bulletSpeed * deltaTime);
        GetTransform().SetPosition(pos);
    }

    void DefaultBullet::CheckBounds()
    {
        const Vec2& bulletPos = GetTransform().GetPosition();
        if (m_direction.x >= 0)
        {
            if (bulletPos.x > 1500)
            {
                Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
            }
        }
    }
}
