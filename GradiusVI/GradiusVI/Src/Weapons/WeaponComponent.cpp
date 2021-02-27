#include "gvipch.h"
#include "Weapons/WeaponComponent.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/Transform.h"
#include "Bullets/LaserBullet.h"
#include "Core/ResourceManager.h"
#include "Components/PrimitiveShapeComponent.h"

namespace Gradius
{
    WeaponComponent::WeaponComponent(Helios::Entity& owner, const Helios::ComponentID& id)
        : ScriptableComponent(owner, id), m_activeLaserBullet(nullptr)
    {
    }

    WeaponComponent::~WeaponComponent()
    {
        for (auto& bullet : m_bulletsShoot)
        {
            bullet->m_onDestroyCallback = nullptr;
        }
        m_bulletsShoot.clear();
    }

    void WeaponComponent::Start()
    {
        m_availableBullets = m_weaponData.m_maxBullets;
    }

    void WeaponComponent::Update(float)
    {
        if (!m_canShoot)
        {
            if (m_fireRateTime.IsFinished())
            {
                m_canShoot = true;
            }
        }
    }

    bool WeaponComponent::Fire()
    {
        if (!m_canShoot || m_availableBullets <= 0 || m_activeLaserBullet != nullptr)
        {
            return false;
        }

        Helios::Entity* bullet = m_weaponData.m_bulletPrefab->Spawn();
        auto baseBullet = bullet->GetComponent<BaseBullet>();

        if (baseBullet != nullptr)
        {
            if (baseBullet->GetBulletType() == EBulletType::Laser)
            {
                m_activeLaserBullet = dynamic_cast<LaserBullet*>(baseBullet);
                bullet->GetTransform().SetParent(&m_owner->GetTransform());

                m_canShoot = false;
                m_fireRateTime.Start(m_weaponData.m_reloadTime);
                m_activeLaserBullet->m_onDestroyCallback = [this](BaseBullet&)
                {
                    m_activeLaserBullet = nullptr;
                };
            }
            else
            {
                m_bulletsShoot.emplace_back(baseBullet);
                bullet->GetTransform().SetPosition(m_nozzle + GetTransform().GetPosition());

                m_canShoot = false;
                m_fireRateTime.Start(m_weaponData.m_reloadTime);
                m_availableBullets--;

                baseBullet->m_onDestroyCallback = [this](BaseBullet& bullet)
                {
                    m_availableBullets++;
                    m_bulletsShoot.erase(std::find(m_bulletsShoot.begin(), m_bulletsShoot.end(), &bullet));
                };
            }
            return true;
        }
        return false;
    }

    void WeaponComponent::StopFire()
    {
        if (m_activeLaserBullet != nullptr)
        {
            m_activeLaserBullet->StopFire();
            m_activeLaserBullet = nullptr;
        }
    }

}
