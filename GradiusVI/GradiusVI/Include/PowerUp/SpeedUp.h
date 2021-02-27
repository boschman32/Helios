#pragma once
#include "BasePowerUp.h"

namespace Gradius
{
    constexpr unsigned int m_maxSpeedLvl = 5;

    class SpeedUp : public BasePowerUp
    {
    public:
        SpeedUp() : BasePowerUp(
            "Speed_Up_Selected", 
            "Speed_Up_Unselected", 
            "./Assets/Sounds/Power_ups/SPEEDUP.wav"){};

        void Activate() override;
        void ResetPowerUp() override;
    private:
        unsigned int m_speedLvl = 0;
        const float m_addedSpeed = 50;
    };
}
