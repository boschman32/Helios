#include "gvipch.h"
#include "Ships/BaseShip/BaseShip.h"
#include "Components/AudioSource.h"
#include "Core/EntityComponent/Entity.h"

namespace Gradius
{
    void BaseShip::Start()
    {
        for (Helios::Transform* child : GetTransform().GetChildren())
        {
            if (child->GetOwner().GetName() == "Model")
            {
                m_model = child->GetOwner().GetComponent<Helios::RenderComponent>();
            }
        }
        m_audioSource = GetOwner().GetComponent<Helios::AudioSource>();
    }

    void BaseShip::SetDropPowerUp(bool a_drop)
    {
        m_canDropPowerUp = a_drop;   
    }

    void Gradius::BaseShip::DamageShip(int a_damage)
    {
        m_shipHealth -= a_damage;
        CheckHealth();
    }

    void Gradius::BaseShip::CheckHealth()
    {
        if (m_shipHealth <= 0)
        {
            DestroyShip();
        }
    }
}