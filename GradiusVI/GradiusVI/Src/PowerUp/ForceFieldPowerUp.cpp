#include "gvipch.h"
#include "PowerUp/ForceFieldPowerUp.h"
#include "Ships/Player/Player.h"

namespace Gradius
{
    ForceFieldPowerUp::ForceFieldPowerUp()
        : BasePowerUp(
            "Force_Field_Selected",
            "Force_Field_Unselected",
            "./Assets/Sounds/Power_ups/FORCEFIELD.wav")
    {
    }

    void ForceFieldPowerUp::Activate()
    {
        m_audioSource->SetFile(m_audioPath);
        m_audioSource->Play();

        m_playerRef->EnableShield();

        DisablePowerUp();
        Unselected();
    }

    void ForceFieldPowerUp::ResetPowerUp()
    {
        m_selectedSprite = "Force_Field_Selected";
        m_unselectedSprite = "Force_Field_Unselected";
        BasePowerUp::ResetPowerUp();
    }

}