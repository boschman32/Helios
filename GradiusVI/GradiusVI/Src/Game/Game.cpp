#include "gvipch.h"
#include "Game/Game.h"
#include "ComponentRegistrations.h"

#include <Helios.h>

#include "Components/AudioSource.h"
#include "Ships/Player/Player.h"
#include "UI/PlayerLivesUI.h"
#include "Core/Prefab.h"

namespace Gradius
{
    // UI Layer
	UILayer::UILayer()
		: Layer("UI Layer")
        , m_playerLivesUI(nullptr)
	{
	}

	void UILayer::OnAttach()
	{

    }

	void UILayer::OnPlay()
	{
        // UI
        m_playerLivesUI = Helios::EntityManager::GetInstance().CreateEntity("PlayerLives");
        m_playerLivesUI->AddComponent<PlayerLivesUI>()->UpdateLives(Player::GetMaxLives());
	}

	void UILayer::OnDetach()
	{

	}

    // Game
	Game::Game()
	{
		PushOverlay(std::make_unique<UILayer>());
    }
}

//Create the game "Application"
Helios::Application* Helios::CreateApplication()
{
	return new Gradius::Game();
}
