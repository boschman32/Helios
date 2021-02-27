#pragma once
#include "Ships/BaseShip/BaseShip.h"

namespace Helios
{
    class QuadCollider;
}

namespace Gradius
{
    class Player;
    class Rugal : public BaseShip
    {
        RTTR_ENABLE(BaseShip, Component);
    public:
        Rugal(Helios::Entity& a_owner, Helios::ComponentID a_id) : BaseShip(a_owner, a_id) {};
        void Start() override;
        void Update(float deltaTime) override;
        void OnOverlapBegin(Helios::Collider* a_collider) override;

        Player* m_playerRef = nullptr;
    private:
        void DestroyShip() override;
        void Seek(float deltaTime);
        Helios::QuadCollider* m_collider = nullptr;

        glm::vec3 m_targetVelocity = glm::vec3(-1, 0, 0);
    };
}
