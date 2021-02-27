#include "gvipch.h"
#include "Ships/Enemies/BigCoreGate.h"
#include "Collider/ColliderTags.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/Colliders/QuadCollider.h"
#include "Components/AudioSource.h"

namespace Gradius
{
    void BigCoreGate::Start()
    {
        BaseShip::Start();
        m_collider = GetOwner().GetComponent<Helios::QuadCollider>();
        m_collider->SetTag(Tags::m_enemyTag);
    }

    void BigCoreGate::DestroyShip()
    {
        m_audioSource->Play();
        m_onShipDestroyedCallback(this);
        Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
    }
}
