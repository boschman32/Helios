#pragma once
#include "BasePowerUp.h"

namespace Gradius
{
    class MissilePowerUp : public BasePowerUp
    {
    public:
        MissilePowerUp() : BasePowerUp(
            "Missile_Selected", 
            "Missile_Unselected", 
            "./Assets/Sounds/Power_ups/MISSILE.wav") {};
        void Activate() override;
        void ResetPowerUp() override;
    };
}
