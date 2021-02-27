#pragma once
#include "Bullets/BaseBullet.h"
#include "Core/Prefab.h"
#include "Core/AudioAsset.h"

namespace Gradius
{
    enum class EWeaponType
    {
        None,
	    DefaultWeapon,
	    Missile,
        Double,
	    Laser,
        BigCore
    };

    class WeaponData
    {
        RTTR_ENABLE()
    public:
        EWeaponType m_weaponType                            { EWeaponType::None };
        
        float m_reloadTime                                  { 0.f };
        int m_maxBullets                                    { 1 };
        std::shared_ptr<Helios::Prefab> m_bulletPrefab      { nullptr };
        Helios::AudioAsset m_shootSound;
    };
}
