#pragma once
#include "Components/ScriptableComponent.h"
#include "Components/Colliders/Collider.h"

namespace Gradius
{
    class KillBox : public Helios::ScriptableComponent
    {
        RTTR_ENABLE(Component,Helios::ScriptableComponent)
    public:
        KillBox(Helios::Entity& owner, Helios::ComponentID id) : ScriptableComponent(owner, id) {};
        void OnOverlapBegin(Helios::Collider*) override;
    };
}

