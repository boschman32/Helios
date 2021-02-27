#include "hepch.h"
#include "Components/Colliders/CircleCollider.h"

namespace Helios
{
	CircleCollider::CircleCollider(Entity& a_owner, const ComponentID& a_id) : Collider(1, a_owner, a_id)
	{
		m_vertices.emplace_back(GetTransform().GetPosition());
		m_radius = 16;
		m_size = Vec2(m_radius * 0.05f, m_radius * 0.05f);
		m_mesh = Mesh::CreateSphere(m_radius * 2.f);
	}

	void CircleCollider::SetRadius(float a_radius)
	{
		m_radius = a_radius;
		m_oldRadius = m_radius;
		m_size = Vec2(m_radius * 0.05f, m_radius * 0.05f);
	}

	float const& CircleCollider::GetRadius() const
	{
		return m_radius;
	}

	void CircleCollider::Render()
	{
		OPTICK_EVENT()

		m_vertices[0].x = GetTransform().GetPosition().x + m_offset.x;
		m_vertices[0].y = GetTransform().GetPosition().y + m_offset.y;
		if (m_radius != m_oldRadius)
		{
			SetRadius(m_radius);
		}
		m_currentPosition = GetTransform().GetPosition();
	}

	Mesh* CircleCollider::GetDrawableShape()
	{
		return m_mesh.get();
	}
}
