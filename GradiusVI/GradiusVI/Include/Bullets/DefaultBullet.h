#pragma once
#include "BaseBullet.h"
#include "Components/ScriptableComponent.h"

namespace Gradius
{
    class DefaultBullet : public BaseBullet
    {
        RTTR_ENABLE(BaseBullet)
    public:
        DefaultBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id);

        void Start() override;
        void Update(float deltaTime) override;
        void OnOverlapBegin(Helios::Collider*) override;

        EBulletType GetBulletType() const override { return EBulletType::DefaultBullet; }
    private:
        void Move(float deltaTime) override;
        void CheckBounds();
    };
}
