#pragma once
#include "Ships/BaseShip/BaseShip.h"
#include "Utils/Timer.h"


namespace Helios
{
    class PolygonCollider;
}

namespace Gradius
{
    enum class EBigCoreState
    {
        Idle,
        Intro,
        MoveForward,
        MoveBackward,
        RollUp,
        RollDown,
        Shoot
    };

    class WeaponComponent;
    class Player;
    class BigCore : public BaseShip
    {
        RTTR_ENABLE(BaseShip, Component);
    public:
        BigCore(Helios::Entity& a_owner, Helios::ComponentID a_id) : BaseShip(a_owner, a_id) {};
        void Start() override;
        void Update(float deltaTime) override;
        void OnOverlapBegin(Helios::Collider* a_col) override;

        Player* m_playerRef = nullptr;
    private:
        void FireWeapons();
        void ChangeState(EBigCoreState a_newState);
        void MoveWithDirection(glm::vec3 a_dir, float deltaTime);
        void DestroyShip() override;
        void GetRollDirection();

        bool m_hasRolled = false;
        bool m_shootWeapons = false;
        bool m_hasMoved = false;
        const float m_rollDistance = 250;
        const float m_moveDistance = 300;
        EBigCoreState m_currentState;
        EBigCoreState m_lastMoveState;
        glm::vec3 m_targetPosition;
        glm::vec3 m_previousPosition;
        Helios::Timer<float> m_idleTimer;
        float m_idleTimeAmount = 0.75f;

        std::vector<Helios::PolygonCollider*> m_shipCollider;
        std::vector<WeaponComponent*> m_weaponComponents;
    };
}
