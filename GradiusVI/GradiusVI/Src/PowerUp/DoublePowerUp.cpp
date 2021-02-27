#include "gvipch.h"
#include "PowerUp/DoublePowerUp.h"
#include "Ships/Player/Player.h"

namespace Gradius
{
    void Gradius::DoublePowerUp::Activate()
    {
        for (WeaponComponent* weapon : m_playerRef->m_weaponComponents)
        {
            if (weapon->GetWeaponType() == EWeaponType::Laser)
            {
                weapon->SetEnabled(false);
            }
            if (weapon->GetWeaponType() == EWeaponType::Double)
            {
                weapon->SetEnabled(true);
            }
        }

        m_audioSource->SetFile(m_audioPath);
        m_audioSource->Play();
        DisablePowerUp();
        Unselected();
    }

    void DoublePowerUp::ResetPowerUp()
    {
        for (WeaponComponent* weapon : m_playerRef->m_weaponComponents)
        {
            if (weapon->GetWeaponType() == EWeaponType::Double)
            {
                weapon->SetEnabled(false);
            }
        }

        m_selectedSprite = "Double_Selected";
        m_unselectedSprite = "Double_Unselected";
        BasePowerUp::ResetPowerUp();
    }                      
}