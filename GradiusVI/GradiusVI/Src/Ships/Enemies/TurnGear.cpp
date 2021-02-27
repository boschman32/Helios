#include "gvipch.h"
#include "Ships/Enemies/TurnGear.h"
#include "Collider/ColliderTags.h"
#include "Components/Colliders/CircleCollider.h"
#include "Components/AudioSource.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/EntityManager.h"

namespace Gradius
{
	void TurnGear::Start()
	{
		BaseShip::Start();
		m_collider = m_owner->GetComponent<Helios::CircleCollider>();
		m_collider->SetTag(Tags::m_enemyTag);
	}

	void TurnGear::Update(float deltaTime)
	{
		GetTransform().Rotate(m_rotationSpeed * deltaTime, Vec3{0.f, 0.f, 1.f});

		Vec3 pos = GetTransform().GetPosition();
		pos.x -= m_shipSpeed * deltaTime;
		GetTransform().SetPosition(pos);
	}

	void TurnGear::OnOverlapBegin(Helios::Collider* a_collider)
	{
		if (a_collider->GetName() == Tags::m_playerTag)
		{
			a_collider->GetOwner().GetComponent<BaseShip>()->DamageShip(1);
			DestroyShip();
		}
	}

	void TurnGear::DestroyShip()
	{
		m_audioSource->m_streaming = false;
		m_audioSource->SetDecibelOffset(-15.0f);
		m_audioSource->SetFile("./Assets/Sounds/Misc/ExplosionTIE.WAV");
		m_audioSource->Play();
        m_onShipDestroyedCallback(this);
		Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
	}
}
