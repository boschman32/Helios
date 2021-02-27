#pragma once
#include "Components/Colliders/Collider.h"
#include "Components/Transform.h"
namespace Helios
{
	class PolygonCollider : public Collider
	{
		RTTR_ENABLE(Component,Collider)
		RTTR_REGISTRATION_FRIEND

	public:
		PolygonCollider(Entity& owner, const ComponentID& a_id);
		void SetSize(float a_x, float a_y);
		void Render() override;

        EColliderTypes GetColliderType() const override { return EColliderTypes::POLYGON; }
		
		Mesh * GetDrawableShape() override;
	};
}