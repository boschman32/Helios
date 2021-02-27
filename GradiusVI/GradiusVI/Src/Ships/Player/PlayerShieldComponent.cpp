#include "gvipch.h"
#include "Ships/Player/PlayerShieldComponent.h"
#include "Collider/ColliderTags.h"
#include "Components/Colliders/Collider.h"
#include "Core/EntityComponent/Entity.h"
#include "Ships/Player/Player.h"

namespace Gradius
{
	PlayerShieldComponent::PlayerShieldComponent(Helios::Entity& a_owner, const Helios::ComponentID& a_id)
		: ScriptableComponent(a_owner, a_id), m_player(nullptr), m_isShieldEnabled(false)
	{
		
	}

	void PlayerShieldComponent::Start()
	{
		
	}

	void PlayerShieldComponent::EnableShield()
	{
		m_isShieldEnabled = true;
		m_owner->SetEnabled(true);
		m_shieldHealth = m_maxShieldHealth;
	}

	void PlayerShieldComponent::DisableShield()
	{
		m_isShieldEnabled = false;
		m_shieldHealth = 0.f;
		m_owner->SetEnabled(false);
	}

	void PlayerShieldComponent::OnOverlapBegin(Helios::Collider* a_collider)
	{
		if (a_collider->GetName() == Tags::m_enemyTag)
		{
			m_shieldHealth -= m_directDamageToShield;
			a_collider->GetOwner().GetComponent<BaseShip>()->DamageShip(999);
			if (m_shieldHealth <= 0.f)
			{
				DisableShield();
			}
		}
		else if (a_collider->GetName() == Tags::m_bulletTag)
		{
			m_shieldHealth -= m_bulletDamageToShield;
			if (m_shieldHealth <= 0.f)
			{
				DisableShield();
			}
		}
	}
}
