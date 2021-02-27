#pragma once
#include "Ships/BaseShip/BaseShip.h"

namespace Helios
{
    class CircleCollider;
}

namespace Gradius
{
    class TurnGear : public BaseShip
    {
        RTTR_ENABLE(BaseShip, Component);
    public:
        TurnGear(Helios::Entity & a_owner, Helios::ComponentID a_id) : BaseShip(a_owner, a_id) {};
        void Start() override;
        void Update(float) override;
        void OnOverlapBegin(Helios::Collider* a_collider) override;
    private:
        void DestroyShip() override;
        Helios::CircleCollider* m_collider = nullptr;
        float m_rotationSpeed = 0;

        RTTR_REGISTRATION_FRIEND;
    };
};