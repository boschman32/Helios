#include "gvipch.h"
#include "Ships/Enemies/BigCore.h"
#include "Collider/ColliderTags.h"
#include "Components/Colliders/PolygonCollider.h"
#include "Components/AudioSource.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Ships/Player/Player.h"
#include "Weapons/WeaponComponent.h"

namespace Gradius
{
    void BigCore::Start()
    {
        BaseShip::Start();
        m_shipCollider = GetOwner().GetComponentsByType<Helios::PolygonCollider>();
        m_shipCollider[0]->SetTag(Tags::m_enemyTag);
        m_shipCollider[1]->SetTag(Tags::m_enemyTag);

        m_weaponComponents = m_owner->GetComponentsByType<WeaponComponent>();
        ChangeState(EBigCoreState::Intro);
    }

    void BigCore::Update(float deltaTime)
    {
        switch (m_currentState)
        {
            case EBigCoreState::Intro:
            {
                if (GetTransform().GetPosition().x > 1000)
                {
                    MoveWithDirection(glm::vec3(-1, 0, 0), deltaTime);
                }
                else
                {
                    GetRollDirection();
                }
                break;
            }
            case EBigCoreState::Idle:
            {
                if (m_idleTimer.IsFinished())
                {
                    if (m_shootWeapons)
                    {
                        ChangeState(EBigCoreState::Shoot);
                    }
                    else if (m_hasRolled)
                    {
                        if (m_lastMoveState == EBigCoreState::RollDown)
                        {
                            m_targetPosition = m_previousPosition - m_rollDistance;
                            ChangeState(EBigCoreState::RollUp);
                        }
                        else
                        {
                            m_targetPosition = m_previousPosition + m_rollDistance;
                            ChangeState(EBigCoreState::RollDown);
                        }
                        m_hasRolled = false;
                    }
                    else if (!m_hasMoved)
                    {
                        m_targetPosition = m_previousPosition - m_moveDistance;
                        ChangeState(EBigCoreState::MoveForward);
                    }
                    else if (m_hasMoved && m_lastMoveState == EBigCoreState::MoveForward)
                    {
                        m_targetPosition = m_previousPosition + m_moveDistance;
                        ChangeState(EBigCoreState::MoveBackward);
                    }
                    else
                    {
                        GetRollDirection();
                    }
                }
                break;
            }
            case EBigCoreState::MoveForward:
            {
                if (GetTransform().GetPosition().x >= m_targetPosition.x)
                {
                    MoveWithDirection(glm::vec3(-1, 0, 0), deltaTime);
                }
                else
                {
                    m_previousPosition = GetTransform().GetPosition();
                    m_shootWeapons = true;
                    m_hasMoved = true;
                    ChangeState(EBigCoreState::Idle);
                }
                break;
            }
            case EBigCoreState::MoveBackward:
            {
                if (GetTransform().GetPosition().x <= m_targetPosition.x)
                {
                    MoveWithDirection(glm::vec3(1, 0, 0), deltaTime);
                }
                else
                {
                    m_shootWeapons = true;
                    m_previousPosition = GetTransform().GetPosition();
                    ChangeState(EBigCoreState::Idle);
                }
                break;
            }
            case EBigCoreState::RollUp:
            {
                if (GetTransform().GetPosition().y >= m_targetPosition.y)
                {
                    MoveWithDirection(glm::vec3(0, -1, 0), deltaTime);
                    if (GetTransform().GetRotation().x <= 180)
                    {
                        GetTransform().Rotate(55 * deltaTime, Vec3{ 1.f, 0.f, 0.f });
                    }
                }
                else
                {
                    GetTransform().SetRotation(glm::vec3(-180, 0, 0));
                    m_previousPosition = GetTransform().GetPosition();
                    m_shootWeapons = true;
                    ChangeState(EBigCoreState::Idle);
                }
                break;
            }
            case EBigCoreState::RollDown:
            {
                if (GetTransform().GetPosition().y <= m_targetPosition.y)
                {
                    MoveWithDirection(glm::vec3(0, 1, 0), deltaTime);
                    if (GetTransform().GetRotation().x >= -180)
                    {
                        GetTransform().Rotate(-55 * deltaTime, Vec3{ 1.f, 0.f, 0.f });
                    }
                }
                else
                {
                    GetTransform().SetRotation(glm::vec3(180, 0,0 ));
                    m_previousPosition = GetTransform().GetPosition();
                    m_shootWeapons = true;
                    ChangeState(EBigCoreState::Idle);
                }
                break;
            }
            case EBigCoreState::Shoot:
            {
                FireWeapons();
                m_idleTimer.Start(m_idleTimeAmount);
                ChangeState(EBigCoreState::Idle);
                break;
            }
        }
    }

    void BigCore::MoveWithDirection(glm::vec3 a_dir, float deltaTime)
    {
        GetTransform().SetPosition(GetTransform().GetPosition() += (m_shipSpeed * deltaTime) * a_dir);
    }

    void BigCore::GetRollDirection()
    {
        glm::vec3 rollDir;
        if (m_playerRef->GetTransform().GetPosition().y > GetTransform().GetPosition().y)
        {
            rollDir = glm::vec3(0, 1, 0);
        }
        else
        {
            rollDir = glm::vec3(0, -1, 0);
        }

        // Roll up or down based on the player's position.
        if (rollDir.y == 1)
        {
            m_hasMoved = false;
            m_hasRolled = true;
            m_targetPosition = GetTransform().GetPosition() + m_rollDistance;
            ChangeState(EBigCoreState::RollDown);
        }
        else
        {
            m_hasMoved = false;
            m_hasRolled = true;
            m_targetPosition = GetTransform().GetPosition() - m_rollDistance;
            ChangeState(EBigCoreState::RollUp);
        }
    }

    void BigCore::FireWeapons()
    {
        for (WeaponComponent* weapon : m_weaponComponents)
        {
            if (weapon->IsEnabled())
            {
                if (weapon->Fire())
                {
					m_audioSource->SetDecibelOffset(-10.0f);
                    m_audioSource->SetFile("./Assets/Sounds/Player/STAR WARS/LaserTIE.WAV");
                    m_audioSource->Play();
                }
            }
        }

        m_shootWeapons = false;
    }

    void BigCore::ChangeState(EBigCoreState a_newState)
    {
        if (m_currentState == EBigCoreState::MoveBackward ||
            m_currentState == EBigCoreState::MoveForward ||
            m_currentState == EBigCoreState::RollUp ||
            m_currentState == EBigCoreState::RollDown)
        {
            m_lastMoveState = m_currentState;
        }
        m_currentState = a_newState;
    }

    void BigCore::OnOverlapBegin(Helios::Collider* a_collider)
    {
        if (a_collider->GetName() == Tags::m_playerTag)
        {
            a_collider->GetOwner().GetComponent<BaseShip>()->DamageShip(1);
        }
    }

    void BigCore::DestroyShip()
    {
        m_audioSource->Play();
        m_onShipDestroyedCallback(this);
        Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
    }
}
