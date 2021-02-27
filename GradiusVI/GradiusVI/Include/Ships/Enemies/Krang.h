#pragma once
#include "Ships/BaseShip/BaseShip.h"

namespace Helios
{
    class QuadCollider;
}

namespace Gradius
{
    class Krang : public BaseShip
    {
        RTTR_ENABLE(BaseShip, Component);
    public:
        Krang(Helios::Entity& a_owner, Helios::ComponentID a_id) : BaseShip(a_owner, a_id) {};
        void Start() override;
        void Update(float deltaTime) override;
        void OnOverlapBegin(Helios::Collider* a_collider) override;
    private:
        void DestroyShip() override;

        Helios::QuadCollider* m_collider = nullptr;
        float m_frequency = 50.0f;  // Speed of sine wave
        float m_amplitude = 0.025f;   // Size of sine wave
        float m_startYpos;

        RTTR_REGISTRATION_FRIEND;
    };
};

