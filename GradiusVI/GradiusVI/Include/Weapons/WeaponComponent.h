#pragma once
#include "Components/ScriptableComponent.h"
#include "WeaponData.h"
#include "Utils/Timer.h"

namespace Gradius
{
    class Player;
    class LaserBullet;

    class WeaponComponent : public Helios::ScriptableComponent
    {
        RTTR_ENABLE(Helios::ScriptableComponent)
    public:
        WeaponComponent(Helios::Entity& owner, const Helios::ComponentID& id);
        ~WeaponComponent() override;

        void Start() override;
        void Update(float) override;
        bool Fire();
        void StopFire();

        constexpr EWeaponType GetWeaponType() const
        {
            return m_weaponType;
        }

        glm::vec3 m_nozzle;
        std::vector<BaseBullet*> m_bulletsShoot;
        std::string m_weaponSound;
    private:
        WeaponData m_weaponData;
        EWeaponType m_weaponType = EWeaponType::DefaultWeapon;
        Helios::Timer<float> m_fireRateTime;
        LaserBullet* m_activeLaserBullet;

        bool m_canShoot { true };
        int m_availableBullets = 0;

        RTTR_REGISTRATION_FRIEND;
    };
}
