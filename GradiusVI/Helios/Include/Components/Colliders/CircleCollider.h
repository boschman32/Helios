#pragma once
#include "Components/Colliders/Collider.h"
#include "Components/Transform.h"
namespace Helios
{
	class CircleCollider : public Collider
	{
		RTTR_ENABLE(Component,Collider)
		RTTR_REGISTRATION_FRIEND

	public:
		CircleCollider(Entity& a_owner, const ComponentID& a_id);
		void SetRadius(float a_radius);
		float const& GetRadius() const;
		void Render() override;

        EColliderTypes GetColliderType() const override { return EColliderTypes::CIRCLE; };

		Mesh * GetDrawableShape() override;
		float m_oldRadius;
	};
}