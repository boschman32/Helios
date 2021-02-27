#pragma once
#include "Components/ScriptableComponent.h"
#include "Components/Colliders/QuadCollider.h"

namespace Gradius
{
    class PowerUpCapsule : public Helios::ScriptableComponent
    {
        RTTR_ENABLE(Component,Helios::ScriptableComponent)
    public:
        PowerUpCapsule(Helios::Entity& owner, Helios::ComponentID id) : ScriptableComponent(owner, id) {};

        void Start() override;
        void Update(float deltaTime) override;

        void OnOverlapBegin(Helios::Collider*) override;
    private:
        Helios::QuadCollider* m_collider = nullptr;
    };
}
