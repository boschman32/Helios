#include "hepch.h"
#include "Components/Colliders/PolygonCollider.h"
#include "Components/Transform.h"
namespace Helios
{
	PolygonCollider::PolygonCollider(Entity& owner, const ComponentID& a_id) : Collider(owner, a_id, 1)
	{
		SetSize(32, 32);
	}

	void PolygonCollider::SetSize(float a_x, float a_y)
	{
		m_polygons[0].SetVertexPosition(0,glm::vec3());
		m_polygons[0].SetVertexPosition(1, glm::vec3(a_x,0,0));
		m_polygons[0].SetVertexPosition(2, glm::vec3(a_x,a_y,0));

		m_polygons[0].SetOrigin(a_x / 2, a_y / 2);


		m_size.x = a_x;
		m_size.y = a_y;

		m_oldSize = m_size;
		std::vector<glm::vec3> vertices;
		for(int i = 0; i < 3;i++)
		{
			m_vertices.push_back(m_polygons[0].GetPoly().m_vertices[i]);
		}
		m_mesh = std::move(Mesh::CreateTriangle(m_size.x / 2,m_size.y / 2));
	}



	void PolygonCollider::Render()
	{
		m_polygons[0].SetPosition(GetTransform().GetPosition().x + m_offset.x, GetTransform().GetPosition().y + m_offset.y);
		m_polygons[0].SetRotation(GetTransform().GetPosition().z + m_offsetRotation);
		m_polygons[0].SetScale(GetTransform().GetScale().x, GetTransform().GetScale().y);

		if(m_oldSize != m_size)
		{
			SetSize(m_size.x, m_size.y);
		}
	}

	Mesh* PolygonCollider::GetDrawableShape()
	{
		return m_mesh.get();
	}
}
