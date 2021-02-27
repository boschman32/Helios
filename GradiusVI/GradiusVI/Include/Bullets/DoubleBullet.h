#pragma once
#include "BaseBullet.h"
#include "Components/ScriptableComponent.h"

namespace Gradius
{
    class DoubleBullet : public BaseBullet
    {
        RTTR_ENABLE(BaseBullet)
    public:
        DoubleBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id);

        void Start() override;
        void Update(float deltaTime) override;
        void OnOverlapBegin(Helios::Collider*) override;

        EBulletType GetBulletType() const override { return EBulletType::Double; }
    private:
        void Move(float deltaTime) override;
        void CheckBounds();
    };
}