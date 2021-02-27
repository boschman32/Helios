#include "gvipch.h"
#include "Ships/Enemies/Rugal.h"
#include "Ships/Player/Player.h"
#include "Collider/ColliderTags.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/Colliders/QuadCollider.h"
#include "Components/AudioSource.h"
#include "glm/geometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Ships/Player/Player.h"

namespace Gradius
{
    void Rugal::Start()
    {
        BaseShip::Start();
        m_collider = m_owner->GetComponent<Helios::QuadCollider>();
        m_collider->SetTag(Tags::m_enemyTag);

        m_targetVelocity = glm::vec3(-1, 0, 0);
    }

    void Rugal::Update(float deltaTime)
    {
        Seek(deltaTime);
    }

    void Rugal::OnOverlapBegin(Helios::Collider* a_collider)
    {
        if (a_collider->GetName() == Tags::m_playerTag)
        {
            a_collider->GetOwner().GetComponent<BaseShip>()->DamageShip(1);
            DestroyShip();
        }
    }

    void Rugal::Seek(float deltaTime)
    {
        if (m_playerRef != nullptr && m_playerRef->GetPlayerState() == EPlayerState::Alive)
        {
            // Get the positions.
            const glm::vec3 myPosition = GetTransform().GetPosition();
            const glm::vec3 playerPosition = m_playerRef->GetOwner().GetComponent<Helios::Transform>()->GetPosition();

            // Move towards target.
            const glm::vec3 desiredVelocity = glm::normalize(myPosition - playerPosition) * m_shipSpeed;
            m_targetVelocity = desiredVelocity;

            // Rotate towards target.
            const glm::vec2 dPos = myPosition - playerPosition;
            const float rotation = atan2(dPos.y, dPos.x) * (90.f / glm::pi<float>());

            GetTransform().SetRotation(rotation, { 0.f, 0.f, 1.f });
        }
        GetTransform().SetPosition(GetTransform().GetPosition() - m_targetVelocity * deltaTime);
    }

    void Rugal::DestroyShip()
    {
		m_audioSource->m_streaming = false;
		m_audioSource->SetDecibelOffset(-15.0f);
		m_audioSource->SetFile("./Assets/Sounds/Misc/ExplosionTIE.WAV");
        m_audioSource->Play();
        m_onShipDestroyedCallback(this);
        Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
    }
}
