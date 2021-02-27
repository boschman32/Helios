#pragma once
#include "BasePowerUp.h"

namespace Gradius
{
    class LaserPowerUp : public BasePowerUp
    {
    public:
        LaserPowerUp();
        void Activate() override;
        void ResetPowerUp() override;
    };
}
#pragma once
