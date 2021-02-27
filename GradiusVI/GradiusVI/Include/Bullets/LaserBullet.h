#pragma once
#include "BaseBullet.h"
#include "Components/ScriptableComponent.h"

namespace Gradius
{
	class LaserBullet : public BaseBullet
	{
        RTTR_ENABLE(BaseBullet)
	public:
		LaserBullet(Helios::Entity& a_owner, const Helios::ComponentID& a_id);

		void OnCreate() override;
        void Start() override;
        void Update(float a_deltaTime) override;
        void OnOverlapBegin(Helios::Collider* a_collider) override;

        void StopFire();
		bool IsFiring() const { return m_isFiring; };

        EBulletType GetBulletType() const override { return EBulletType::Laser; }
    private:
        void Move(float a_deltaTime) override;
        void CheckBounds();

        float m_laserMinSizeX;
        float m_laserMaxSizeX;
        float m_laserScaleSpeed;
        bool m_isFiring;

        RTTR_REGISTRATION_FRIEND
	};
}
