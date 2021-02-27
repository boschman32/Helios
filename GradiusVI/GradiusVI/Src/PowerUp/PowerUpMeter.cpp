#include "gvipch.h"
#include "PowerUp/PowerUpMeter.h"
#include "PowerUp/SpeedUp.h"
#include "PowerUp/MissilePowerUp.h"
#include "PowerUp/DoublePowerUp.h"
#include "Ships/Player/Player.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/Transform.h"
#include "Components/UIComponent.h"
#include "Components/AudioSource.h"
#include "PowerUp/ForceFieldPowerUp.h"
#include "PowerUp/LaserPowerUp.h"

namespace Gradius
{
    void PowerUpMeter::Start()
    {
        InitializePowerUps();

        // Needs to be smarter for multiple power up types.
        for (unsigned int i = 0; i < m_maxPowerUps; i++)
        {
            m_powerUpEntity[i] = Helios::EntityManager::GetInstance().CreateEntity("Power Up");
            m_powerUpEntity[i]->GetTransform().SetPosition((1280.f / 4.f) + (m_powerUpSpriteWidth * i), 600.f, 0.f);
            m_powerUps[i]->m_image = m_powerUpEntity[i]->AddComponent<Helios::UIComponent>();
            m_powerUps[i]->m_image->m_position.x = -0.28f + 0.1f * static_cast<float>(i);
            m_powerUps[i]->m_image->m_position.y = 0.35f;

            m_powerUps[i]->m_audioSource = m_powerUpEntity[i]->AddComponent<Helios::AudioSource>();
            m_powerUps[i]->Unselected();
            m_powerUps[i]->m_playerRef = GetOwner().GetComponent<Player>();
        }
    }

    void PowerUpMeter::InitializePowerUps()
    {
        m_powerUps[0] = std::make_shared<SpeedUp>();
        m_powerUps[1] = std::make_shared<MissilePowerUp>();
        m_powerUps[2] = std::make_shared<DoublePowerUp>();
        m_powerUps[3] = std::make_shared<LaserPowerUp>();
        m_powerUps[4] = std::make_shared<BasePowerUp>("Empty_Selected", "Empty_Unselected", "");
        m_powerUps[4]->m_selectable = false;
        m_powerUps[5] = std::make_shared<ForceFieldPowerUp>();
    }

    void PowerUpMeter::NextPowerUp()
    {
        // Check if we have a power-up.
        if (m_currentPowerUp != nullptr)
        {
            m_currentPowerUp->Unselected();

            if (m_powerUpCounter < m_maxPowerUps-1)
            {
                m_powerUpCounter++;
                m_currentPowerUp = m_powerUps[m_powerUpCounter];
            }
            else
            {
                m_currentPowerUp = m_powerUps[0];
                m_powerUpCounter = 0;
            }
            m_currentPowerUp->Selected();
        }
        else
        {
            // Power-up doesn't exist so set it to default.
            m_currentPowerUp = m_powerUps[0];
            m_powerUpCounter = 0;
            m_currentPowerUp->Selected();
        }
       
    }

    void PowerUpMeter::ActivateCurrentPowerUp()
    {
        if (m_currentPowerUp != nullptr && m_currentPowerUp->m_selectable)
        {
            m_currentPowerUp->Activate();
            m_currentPowerUp = nullptr;
        }
    }

    void PowerUpMeter::ResetAllPowerUps()
    {
        for (unsigned int i = 0; i < m_powerUps.size(); i++)
        {
            m_powerUps[i]->ResetPowerUp();
        }
        m_currentPowerUp = nullptr;
        m_powerUpCounter = 0;
    }
}
