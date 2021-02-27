#pragma once
#include "Components/ScriptableComponent.h"
#include <Array>

namespace Gradius
{
    class BasePowerUp;

    // Powerup sprite info.
    constexpr float m_powerUpSpriteWidth = 128;
    constexpr unsigned int m_maxPowerUps = 6;

    class PowerUpMeter : public Helios::ScriptableComponent
    {
        RTTR_ENABLE(Component,Helios::ScriptableComponent)
    public:
        PowerUpMeter(Helios::Entity& owner, Helios::ComponentID id) : ScriptableComponent(owner, id) {};

        void Start() override;
        void NextPowerUp();
        void ActivateCurrentPowerUp();
        void ResetAllPowerUps();
    private:
        void InitializePowerUps();
        unsigned int m_powerUpCounter = 0;

        std::array<Helios::Entity*, m_maxPowerUps> m_powerUpEntity;
        std::array<std::shared_ptr<BasePowerUp>, m_maxPowerUps> m_powerUps;
        std::shared_ptr<BasePowerUp> m_currentPowerUp = nullptr;
    };
}
