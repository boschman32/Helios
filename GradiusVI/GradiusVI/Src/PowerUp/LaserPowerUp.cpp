#include "gvipch.h"
#include "PowerUp/LaserPowerUp.h"
#include "Ships/Player/Player.h"

namespace Gradius
{
    LaserPowerUp::LaserPowerUp()
        : BasePowerUp(
            "Laser_Selected",
            "Laser_Unselected",
            "./Assets/Sounds/Power_ups/LASER.wav")
    {
    }

    void LaserPowerUp::Activate()
    {
        for (WeaponComponent* weapon : m_playerRef->m_weaponComponents)
        {
            if(weapon->GetWeaponType() == EWeaponType::Missile && weapon->IsEnabled())
            {
                continue;
            }
            weapon->SetEnabled(false);
            if (weapon->GetWeaponType() == EWeaponType::Laser)
            {
                weapon->SetEnabled(true);
            }
        }

        m_audioSource->SetFile(m_audioPath);
        m_audioSource->Play();
        DisablePowerUp();
        Unselected();
    }

    void LaserPowerUp::ResetPowerUp()
    {
        for (WeaponComponent* weapon : m_playerRef->m_weaponComponents)
        {
            weapon->SetEnabled(false);
        }

        m_selectedSprite = "Laser_Selected";
        m_unselectedSprite = "Laser_Unselected";
        BasePowerUp::ResetPowerUp();
    }

}