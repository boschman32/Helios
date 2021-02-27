#include "gvipch.h"
#include "Ships/Enemies/Krang.h"
#include "Collider/ColliderTags.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/Colliders/QuadCollider.h"
#include "Components/AudioSource.h"

namespace Gradius
{
    void Krang::Start()
    {
        BaseShip::Start();
        m_startYpos = GetTransform().GetPosition().y;

        m_collider = m_owner->GetComponent<Helios::QuadCollider>();
        m_collider->SetTag(Tags::m_enemyTag);
    }

    void Krang::Update(float deltaTime)
    {
        // Move Krang with a sine wave.
        Vec3 pos = GetTransform().GetPosition();
        pos.x -= m_shipSpeed * deltaTime;
        pos.y =
            m_frequency * glm::sin(m_amplitude * pos.x) + m_startYpos;
        GetTransform().SetPosition(pos);
    }

    void Krang::OnOverlapBegin(Helios::Collider* a_collider)
    {
        if (a_collider->GetName() == Tags::m_playerTag)
        {
            a_collider->GetOwner().GetComponent<BaseShip>()->DamageShip(1);
            DestroyShip();
        }
    }

    void Krang::DestroyShip()
    {
		m_audioSource->m_streaming = false;
		m_audioSource->SetDecibelOffset(-15.0f);
		m_audioSource->SetFile("./Assets/Sounds/Misc/ExplosionTIE.WAV");
        m_audioSource->Play();
        m_onShipDestroyedCallback(this);

        Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
    }
}
