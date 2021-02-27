#pragma once
#include "Components/ScriptableComponent.h"

namespace Gradius
{
    class Player;
    class PlayerShieldComponent : public Helios::ScriptableComponent
    {
        RTTR_ENABLE(Component,Helios::ScriptableComponent)
    public:
        PlayerShieldComponent(Helios::Entity& a_owner, const Helios::ComponentID& a_id);
        void Start() override;

        void EnableShield();
        void DisableShield();

    	bool IsShieldEnabled() const { return m_isShieldEnabled; }

        void OnOverlapBegin(Helios::Collider* a_collider) override;
    private:
        Player* m_player;

        float m_maxShieldHealth = 0.f;
        float m_shieldHealth = 0.f;
        float m_bulletDamageToShield = 0.f;
        float m_directDamageToShield = 0.f;
    	bool m_isShieldEnabled;

        RTTR_REGISTRATION_FRIEND;
    };
}
