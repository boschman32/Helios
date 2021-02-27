#include "gvipch.h"
#include "UI/PlayerLivesUI.h"
#include "Ships/Player/Player.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/UIComponent.h"

namespace Gradius
{
    void PlayerLivesUI::Start()
    {
        Player::s_UpdateLivesCallback = [this](int a_playerLives)
        {
            UpdateLives(a_playerLives);
        };
    }

    void PlayerLivesUI::UpdateLives(const int a_playerLives)
    {
        if (a_playerLives < 0)
        {
            return;
        }

        if (m_livesSprites.size() > a_playerLives)
        {
            for (int i = static_cast<int>(m_livesSprites.size()); i-- > a_playerLives; )
            {
                Helios::EntityManager::GetInstance().DestroyEntity(m_livesSprites[i]);
                m_livesSprites.erase(m_livesSprites.begin() + i);
            }
        }

        if (m_livesSprites.empty())
        {
            m_livesSprites.reserve(a_playerLives);

            for (int i = 0; i < a_playerLives; i++)
            {
                m_livesSprites.push_back(Helios::EntityManager::GetInstance().CreateEntity("Live_Icon_" + std::to_string(i)));
                Helios::UIComponent* icon_UI = m_livesSprites[i]->AddComponent<Helios::UIComponent>();
                icon_UI->SetTextureFile("./Assets/Sprites/UI/Player_Live.png");
                icon_UI->m_position.x = -0.45f + (0.05f * static_cast<float>(i));
                icon_UI->m_position.y = -0.45f;
            }
        }
    }
}