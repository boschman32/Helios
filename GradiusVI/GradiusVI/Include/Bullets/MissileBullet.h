#pragma once
#include "BaseBullet.h"
#include "Components/ScriptableComponent.h"

namespace Gradius
{
    class MissileBullet : public BaseBullet
    {
        RTTR_ENABLE(BaseBullet)
    public:
        MissileBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id);

        void Start() override;
        void Update(float deltaTime) override;
        void OnOverlapBegin(Helios::Collider*) override;

        EBulletType GetBulletType() const override { return EBulletType::Missile; }
    private:
        void Move(float deltaTime) override;
        void CheckBounds();
    };
}