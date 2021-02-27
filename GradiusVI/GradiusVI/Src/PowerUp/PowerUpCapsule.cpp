#include "gvipch.h"
#include "PowerUp/PowerUpCapsule.h"
#include "Collider/ColliderTags.h"
#include "Core/EntityComponent/Entity.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Components/MeshRenderer.h"

namespace Gradius
{
    void PowerUpCapsule::Start()
    {
        m_collider = m_owner->GetComponent<Helios::QuadCollider>();
        m_collider->SetTag(Tags::m_powerupCapsuleTag);
    }

    void PowerUpCapsule::Update(float deltaTime)
    {
        glm::vec3 pos = GetTransform().GetPosition();
        pos.x -= 30 * deltaTime;
        GetTransform().SetPosition(pos);
    }

    void PowerUpCapsule::OnOverlapBegin(Helios::Collider* a_collider)
    {
        if (a_collider->GetName() == Tags::m_playerTag)
        {
            Helios::EntityManager::GetInstance().DestroyEntity(&GetOwner());
        }
    }
}
