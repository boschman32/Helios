#include "gvipch.h"
#include "PowerUp/MissilePowerUp.h"
#include "Weapons/WeaponComponent.h"
#include "Core/EntityComponent/Entity.h"
#include "Ships/Player/Player.h"

namespace Gradius
{
    void MissilePowerUp::Activate()
    {
        for (WeaponComponent* weapon : m_playerRef->m_weaponComponents)
        {
            if (weapon->GetWeaponType() == EWeaponType::Missile)
            {
                weapon->SetEnabled(true);
            }
        }

        m_audioSource->SetFile(m_audioPath);
        m_audioSource->Play();
        DisablePowerUp();
        Unselected();
    }

    void MissilePowerUp::ResetPowerUp()
    {
        for (WeaponComponent* weapon : m_playerRef->m_weaponComponents)
        {
            if (weapon->GetWeaponType() == EWeaponType::Missile)
            {
                weapon->SetEnabled(false);
            }
        }

        m_selectedSprite = "Missile_Selected";
        m_unselectedSprite = "Missile_Unselected";
        BasePowerUp::ResetPowerUp();
    }
}
