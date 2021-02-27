#include "gvipch.h"
#include "Bullets/LaserBullet.h"
#include "Collider/ColliderTags.h"
#include "Ships/BaseShip/BaseShip.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/Colliders/QuadCollider.h"
#include "Components/PrimitiveShapeComponent.h"
#include "Core/ResourceManager.h"

namespace Gradius
{
    LaserBullet::LaserBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id)
        : BaseBullet(a_owner, a_id),
        m_laserMinSizeX(1.f),
        m_laserMaxSizeX(20.f),
        m_laserScaleSpeed(80.f)
    {
        if (m_onDestroyCallback != nullptr)
        {
            m_onDestroyCallback(*this);
        }
    }

    void LaserBullet::OnCreate()
    {
       /* auto shape = m_owner->AddComponent<Helios::PrimitiveShape>();
        shape->SetCurrentShapeType(Helios::EShapeType::EShapeType_Plane);
        GetTransform().SetRotation({ -90.f, 0.f, 0.f });
        GetTransform().SetScale({g_laserSpriteSize.x * 0.05f, g_laserSpriteSize.y * 0.05f, 1.f});

        shape->SetTexture(Helios::ResourceManager::GetInstance().LoadResource<Helios::DX12Texture>("./Assets/Sprites/Player/laser_bullet.png"));*/
    }

    void LaserBullet::Start()
    {
        m_collider = m_owner->GetComponent<Helios::QuadCollider>();
        m_collider->SetTag(Tags::m_bulletPlayerTag);
        m_isFiring = true;
    }

    void LaserBullet::Update(float a_deltaTime)
    {
        if (m_isFiring)
        {
            const float laserSize = std::clamp(GetOwner().GetTransform().GetScale().x + (m_laserScaleSpeed * a_deltaTime), m_laserMinSizeX, m_laserMaxSizeX);
            if (laserSize >= m_laserMaxSizeX)
            {
                m_isFiring = false;
            }
            else
            {
                const Vec3 curScale = GetTransform().GetScale();
                GetTransform().SetScale(laserSize, curScale.y, curScale.z);
                m_collider->SetSize(laserSize / 0.05f, m_collider->GetSize().y);
            }
        }
        else
        {
            Move(a_deltaTime);
        }

        CheckBounds();
    }

    void LaserBullet::StopFire()
    {
        m_isFiring = false;
    }

    void LaserBullet::OnOverlapBegin(Helios::Collider* a_collider)
    {
        if (a_collider->GetName() == Tags::m_enemyTag)
        {
            StopFire();
            a_collider->GetOwner().GetComponent<BaseShip>()->DamageShip(m_damage);

            if (a_collider->GetOwner().GetComponent<BaseShip>()->GetShipHealth() > 0)
            {
                Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
            }
        }
    }

    void LaserBullet::Move(float a_deltaTime)
    {
        Vec3 pos = GetOwner().GetTransform().GetLocalPosition();
        pos.x += m_direction.x * (m_bulletSpeed * a_deltaTime);
        pos.y += m_direction.y * (m_bulletSpeed * a_deltaTime);
        GetOwner().GetTransform().SetLocalPosition(pos);
    }

    void LaserBullet::CheckBounds()
    {
        Vec2 bulletPos;
        bulletPos.x = GetOwner().GetTransform().GetPosition().x;
        if (m_direction.x >= 0)
        {
            if (bulletPos.x > 1500)
            {
                StopFire();
                Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
            }
        }
    }
}
