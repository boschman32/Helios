#pragma once
#include "Components/Colliders/Collider.h"
#include "Components/Transform.h"
namespace Helios
{
	class QuadCollider : public Collider
	{
		RTTR_ENABLE(Component, Collider)
		RTTR_REGISTRATION_FRIEND
	public:
		QuadCollider(Entity& a_owner, const ComponentID& a_id);
		void SetSize(float a_x, float a_y);

        EColliderTypes GetColliderType() const override { return EColliderTypes::QUAD; }

		void Render() override;
		Mesh* GetDrawableShape() override;
	};
}