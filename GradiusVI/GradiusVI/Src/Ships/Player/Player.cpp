#include "gvipch.h"
#include "Ships/Player/Player.h"
#include "Collider/ColliderTags.h"
#include "Core/EntityComponent/Entity.h"
#include "Components/AudioSource.h"
#include "Weapons/WeaponComponent.h"
#include "Core/Logger.h"
#include "Ships/Player/PlayerShieldComponent.h"
#include "Core/Application.h"

namespace Gradius
{
	std::function<void(int a_lives)> Player::s_UpdateLivesCallback;

	Player::Player(Helios::Entity& a_owner, Helios::ComponentID a_id)
		: BaseShip(a_owner, a_id),
		m_shield(nullptr), m_playerState(EPlayerState::Alive), m_acceleration(0.f)
	{

	}

	void Player::Start()
	{
		BaseShip::Start();

		// Get all player input.
		SetInput();

		// Set the player lives.
		m_lives = GetMaxLives();
		m_playerState = EPlayerState::Alive;

		// Get the reference of the components.
		m_weaponComponents.reserve(4);
		m_weaponComponents = m_owner->GetComponentsByType<WeaponComponent>();

		m_collider = m_owner->GetComponent<Helios::QuadCollider>();
		m_collider->SetTag(Tags::m_playerTag);

		m_powerupMeter = m_owner->GetComponent<PowerUpMeter>();

		for (auto child : m_owner->GetTransform().GetChildren())
		{
			if (child->GetOwner().GetName() == "Shield")
			{
				m_shield = child->GetOwner().GetComponent<PlayerShieldComponent>();
			}
		}

		//BG Music
		Helios::Entity * music = Helios::EntityManager::GetInstance().CreateEntity("BG MUSIC");
		Helios::AudioSource* musicsource = music->AddComponent<Helios::AudioSource>();
		musicsource->m_streaming = true;
		musicsource->SetFile("./Assets/Sounds/Background/Theraphy.mp3");
		musicsource->m_looping = true;
		musicsource->Play();

		m_audioSource->SetDecibelOffset(-15.0f);
	}

	void Player::Update(float deltaTime)
	{
		switch (m_playerState)
		{
		case EPlayerState::Alive:
		{
			Move(deltaTime);
			FireWeapons();
			ActivatePowerUp();
			break;
		}
		case EPlayerState::Dead:
		{
			if (m_respawnTimer.IsFinished())
			{
                if (m_lives != -1)
                {
                    RespawnShip();
                }
			}
			break;
		}
		case EPlayerState::Invulnerable:
		{
			Move(deltaTime);
			FireWeapons();
			ActivatePowerUp();

			if (m_invulnerableTimer.IsFinished())
			{
				m_playerState = EPlayerState::Alive;
                m_blinkCount = 0;
                m_shipHealth = 1;
				m_collider->SetEnabled(true);

                m_model->SetEnabled(true);
			}
			else
			{
				if (m_blinkCount % 6 == 0)
				{
                    m_model->SetEnabled(true);
				}
				else
				{
                    m_model->SetEnabled(false);
				}
				m_blinkCount++;
            }
			break;
		}
		}
	}

	void Player::DamageShip(int a_damage)
	{
		if(!m_shield->IsShieldEnabled())
		{
			BaseShip::DamageShip(a_damage);
		}
	}

	void Player::OnOverlapBegin(Helios::Collider* a_col)
	{
		if (a_col->GetName() == Tags::m_powerupCapsuleTag)
		{
			m_powerupMeter->NextPowerUp();
		}
	}

	void Player::AddShipSpeed(float a_speed)
	{
		m_shipSpeed += a_speed;
	}

	void Player::EnableShield() const
	{
		if (m_shield != nullptr)
		{
			m_shield->EnableShield();
		}
	}

	void Player::DestroyShip()
	{
		m_playerState = EPlayerState::Dead;
		m_isAlive = false;
		m_lives--;

		m_respawnTimer.Start(m_respawnTime);
		m_collider->SetIsColliding(false);
		m_collider->SetEnabled(false);

        m_model->SetEnabled(false);

		m_audioSource->SetFile("./Assets/Sounds/Player/DEAD.wav");
		m_audioSource->Play();
		m_powerupMeter->ResetAllPowerUps();

		if (s_UpdateLivesCallback != nullptr)
		{
			s_UpdateLivesCallback(m_lives);
		}
	}

	void Player::RespawnShip()
	{
        //Enable the default weapon again.
        m_weaponComponents[0]->SetEnabled(true);
		m_playerState = EPlayerState::Invulnerable;
		m_invulnerableTimer.Start(m_invulnerableTime);
	}

	void Player::Move(float a_deltaTime)
	{
		m_acceleration = (m_shipSpeed * 8) * a_deltaTime;

		if (Helios::InputManager::GetController(1)->GetButtonDown("MoveRight"))
		{
			m_velocity.x += m_acceleration;
		}
		else if (Helios::InputManager::GetController(1)->GetButtonDown("MoveLeft"))
		{
			m_velocity.x -= m_acceleration;
		}
		else
		{
			if (m_velocity.x > m_acceleration)
			{
				m_velocity.x -= m_acceleration;
			}
			else if (m_velocity.x < -m_acceleration)
			{
				m_velocity.x += m_acceleration;
			}
			else
			{
				m_velocity.x = 0;
			}
		}

		if (Helios::InputManager::GetController(1)->GetButtonDown("MoveUp"))
		{
			m_velocity.y -= m_acceleration;

			Vec3 euler = GetTransform().GetRotationInEuler();
			euler.x = std::clamp(euler.x + (m_bankingSpeed * a_deltaTime), 0.f, m_bankingAngle);
			GetTransform().SetRotation(euler);
			
			m_isBanking = true;
		}
		else if (Helios::InputManager::GetController(1)->GetButtonDown("MoveDown"))
		{
			m_velocity.y += m_acceleration;

			Vec3 euler = GetTransform().GetRotationInEuler();
			euler.x = std::clamp(euler.x - (m_bankingSpeed * a_deltaTime), -m_bankingAngle, 0.f);
			GetTransform().SetRotation(euler);

			m_isBanking = true;
		}
		else
		{
			if (m_velocity.y > m_acceleration)
			{
				m_velocity.y -= m_acceleration;
			}
			else if (m_velocity.y < -m_acceleration)
			{
				m_velocity.y += m_acceleration;
			}
			else
			{
				m_velocity.y = 0;
			}

			if(m_isBanking)
			{
				Vec3 euler = GetTransform().GetRotationInEuler();
				if(euler.x < 0.f)
				{
					euler.x = std::clamp(euler.x + (m_bankingSpeed * a_deltaTime), -m_bankingAngle, 0.f);
					GetTransform().SetRotation(euler);
				}
				else if(euler.x > 0.f)
				{
					euler.x = std::clamp(euler.x - (m_bankingSpeed * a_deltaTime), 0.f, m_bankingAngle);
					GetTransform().SetRotation(euler);
				}
				else
				{
					m_isBanking = false;
				}
			}
		
		}

		// Clamp velocity so we won't exceed the max speed.
		m_velocity = glm::clamp(m_velocity, -m_shipSpeed, m_shipSpeed);

		Vec3 pos = GetTransform().GetPosition();
		pos.x += m_velocity.x * a_deltaTime;
		pos.y += m_velocity.y * a_deltaTime;
		GetTransform().SetPosition(ClampInEdge(pos));
	}

	Vec3 Player::ClampInEdge(const Vec3& a_pos) const
	{
		const float x = std::clamp(a_pos.x, m_edgeLeft, m_edgeRight);
		const float y = std::clamp(a_pos.y, m_edgeTop, m_edgeBottom);

		return { x, y, 0.f};
	}

	void Player::FireWeapons()
	{
        if (Helios::InputManager::GetController(1)->GetButtonDown("Fire"))
        {
            for (WeaponComponent* weapon : m_weaponComponents)
            {
                if (weapon->IsEnabled())
                {
                    if (weapon->Fire())
                    {
                        m_audioSource->SetFile("./Assets/Sounds/Player/STAR WARS/LaserXWING.WAV");
                        m_audioSource->Play();
                    }
                }
            }
        }
        else if (Helios::InputManager::GetController(1)->GetButtonUp("Fire"))
        {
            for (WeaponComponent* weapon : m_weaponComponents)
            {
                if (weapon->IsEnabled())
                {
                    weapon->StopFire();
                }
            }
        }
	}

	void Player::ActivatePowerUp()
	{
        if (Helios::InputManager::GetController(1)->GetButtonDown("ActivatePowerUp"))
        {
            if (m_powerupMeter != nullptr)
            {
                m_powerupMeter->ActivateCurrentPowerUp();
            }
        }
	}

	void Player::SetInput()
	{
		// Create the controller
		controller = std::make_unique<Helios::Controller>();

		// Set the input keys.
		Helios::Button fireButton;
		fireButton.AddKey(Key::Space);
		fireButton.SetName("Fire");
		controller->AddButton(fireButton);

		Helios::Button shiftButton;
		shiftButton.AddKey(Key::ShiftKey);
		shiftButton.SetName("ActivatePowerUp");
		controller->AddButton(shiftButton);

		Helios::Button moveRightButton;
		moveRightButton.AddKey(Key::D);
		moveRightButton.AddKey(Key::Right);
		moveRightButton.SetName("MoveRight");
		controller->AddButton(moveRightButton);

		Helios::Button moveLeftButton;
		moveLeftButton.AddKey(Key::A);
		moveLeftButton.AddKey(Key::Left);
		moveLeftButton.SetName("MoveLeft");
		controller->AddButton(moveLeftButton);

		Helios::Button moveUpButton;
		moveUpButton.AddKey(Key::W);
		moveUpButton.AddKey(Key::Up);
		moveUpButton.SetName("MoveUp");
		controller->AddButton(moveUpButton);

		Helios::Button moveDownButton;
		moveDownButton.AddKey(Key::S);
		moveDownButton.AddKey(Key::Down);
		moveDownButton.SetName("MoveDown");
		controller->AddButton(moveDownButton);

		// Add the controller.
		Helios::InputManager::AddController(std::move(controller));
	}
}

