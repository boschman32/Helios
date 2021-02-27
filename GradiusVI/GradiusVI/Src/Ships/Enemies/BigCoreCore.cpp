#include "gvipch.h"
#include "Ships/Enemies/BigCoreCore.h"
#include "Ships/Enemies/BigCore.h"
#include "Components/Colliders/QuadCollider.h"
#include "Components/AudioSource.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Collider/ColliderTags.h"

namespace Gradius
{
    void BigCoreCore::Start()
    {
        BaseShip::Start();
        m_collider = GetOwner().GetComponent<Helios::QuadCollider>();
        m_collider->SetTag(Tags::m_enemyTag);
        m_bigCoreRef = GetOwner().GetTransform().GetRoot()->GetOwner().GetComponent<BigCore>();
    }

    void BigCoreCore::DamageShip(int a_damage)
    {
        m_bigCoreRef->DamageShip(a_damage * 3);
    }
}
