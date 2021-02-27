#pragma once
#include "Components/ScriptableComponent.h"
#include "eventpp/callbacklist.h"
#include "Components/RenderComponent.h"

namespace Helios
{
    class AudioSource;
    class Transform;
}

namespace Gradius
{
    class BaseShip : public Helios::ScriptableComponent
    {
        RTTR_ENABLE(Component,Helios::ScriptableComponent)
    public:
        BaseShip(Helios::Entity& owner, Helios::ComponentID id)
    	: ScriptableComponent(owner, id) {};
        virtual void DamageShip(int);

        void SetDropPowerUp(bool a_drop);

        int GetShipHealth() const { return m_shipHealth; }
        eventpp::CallbackList<void(BaseShip*)> m_onShipDestroyedCallback;
    protected:
        void Start() override;
        virtual void CheckHealth();
        virtual void DestroyShip(){};

        int m_shipHealth = 1;
        float  m_shipSpeed = 1;
        bool m_canDropPowerUp = false;
        Helios::RenderComponent* m_model = nullptr;
        Helios::AudioSource* m_audioSource = nullptr;
        RTTR_REGISTRATION_FRIEND;
    };
}
