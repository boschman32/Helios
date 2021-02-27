#pragma once
#include "Ships/BaseShip/BaseShip.h"

namespace Helios
{
    class QuadCollider;
}

namespace Gradius
{
    class BigCoreGate : public BaseShip
    {
        RTTR_ENABLE(BaseShip, Component);
    public:
        BigCoreGate(Helios::Entity& a_owner, Helios::ComponentID a_id) : BaseShip(a_owner, a_id) {};
        void Start() override;
    private:
        void DestroyShip() override;
        Helios::QuadCollider* m_collider = nullptr;
    };
};