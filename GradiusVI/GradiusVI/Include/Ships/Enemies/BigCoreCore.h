#pragma once
#include "Ships/BaseShip/BaseShip.h"

namespace Helios
{
    class QuadCollider;
}

namespace Gradius
{
    class BigCore;
    class BigCoreCore : public BaseShip
    {
        RTTR_ENABLE(BaseShip, Component);
    public:
        BigCoreCore(Helios::Entity& a_owner, Helios::ComponentID a_id) : BaseShip(a_owner, a_id) {};
        void Start() override;
    private:
        void DamageShip(int a_damage) override;

        BigCore* m_bigCoreRef = nullptr;
        Helios::QuadCollider* m_collider = nullptr;
    };
}
