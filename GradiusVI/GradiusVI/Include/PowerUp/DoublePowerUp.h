#pragma once
#include "BasePowerUp.h"

namespace Gradius
{
    class DoublePowerUp : public BasePowerUp
    {
    public:
        DoublePowerUp() : BasePowerUp(
            "Double_Selected", 
            "Double_Unselected", 
            "./Assets/Sounds/Power_ups/DOUBLE.wav") {};
        void Activate() override;
        void ResetPowerUp() override;
    };
}
