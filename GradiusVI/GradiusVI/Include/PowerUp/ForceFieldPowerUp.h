#pragma once
#include "BasePowerUp.h"

namespace Gradius
{
    class ForceFieldPowerUp : public BasePowerUp
    {
    public:
        ForceFieldPowerUp();
        void Activate() override;
        void ResetPowerUp() override;
    };
}
