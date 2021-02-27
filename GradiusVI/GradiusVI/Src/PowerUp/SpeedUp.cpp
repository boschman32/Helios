#include "gvipch.h"
#include "PowerUp/SpeedUp.h"
#include "Ships/Player/Player.h"

namespace Gradius
{
    void SpeedUp::Activate()
    {
        if (m_speedLvl < m_maxSpeedLvl)
        {
            // Increase player speed.
            m_speedLvl++;
            m_playerRef->AddShipSpeed(m_addedSpeed);

            if (m_speedLvl == m_maxSpeedLvl)
            {
                m_unselectedSprite = "Init_Speed_Unselected";
                m_selectedSprite = "Init_Speed_Selected";
            }
        }
        else
        {
            ResetPowerUp();
        }

        m_audioSource->SetFile(m_audioPath);

        m_audioSource->Play();
        Unselected();
    }

    void SpeedUp::ResetPowerUp()
    {
        // Reset to initial speed.
        m_playerRef->AddShipSpeed(-m_addedSpeed * static_cast<float>(m_speedLvl));
        m_speedLvl = 0;

        m_selectedSprite = "Speed_Up_Selected";
        m_unselectedSprite = "Speed_Up_Unselected";
        BasePowerUp::ResetPowerUp();
    }
}
