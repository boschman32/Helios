#include "gvipch.h"
#include "Collider/ColliderTags.h"
#include "Ships/BaseShip/BaseShip.h"
#include "Bullets/MissileBullet.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/Colliders/QuadCollider.h"
#include "Components/MeshRenderer.h"

namespace Gradius
{
    MissileBullet::MissileBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id)
        : BaseBullet(a_owner, a_id)
    {
        
    }

	void MissileBullet::Start()
	{
		m_collider = m_owner->GetComponent<Helios::QuadCollider>();
		m_collider->SetTag(Tags::m_bulletPlayerTag);

        GetTransform().SetRotation(glm::vec3(0, 0, -135));
	}

	void MissileBullet::OnOverlapBegin(Helios::Collider* a_collider)
	{
		if (a_collider->GetName() == Tags::m_enemyTag)
		{
			a_collider->GetOwner().GetComponent<BaseShip>()->DamageShip(m_damage);
			Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
		}
	}

	void MissileBullet::Update(float deltaTime)
	{
		Move(deltaTime);
		CheckBounds();
	}

	void MissileBullet::Move(float deltaTime)
	{
        Vec3 pos = GetOwner().GetTransform().GetPosition();
        pos.x += m_direction.x * (m_bulletSpeed * deltaTime);
        pos.y += m_direction.y * (m_bulletSpeed * deltaTime);
        GetTransform().SetPosition(pos);
	}

	void MissileBullet::CheckBounds()
	{
		Vec2 bulletPos;
		bulletPos.y = GetOwner().GetComponent<Helios::Transform>()->GetPosition().y;
		if (m_direction.y >= 1)
		{
			if (bulletPos.y > 900)
			{
				Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
			}
		}
	}
}
