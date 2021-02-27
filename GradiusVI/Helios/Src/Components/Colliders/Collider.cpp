#include "hepch.h"
#include "Components/Colliders/Collider.h"
#include "Core/EntityComponent/Entity.h"
#include "Utils/TriangleMath.h"

namespace Helios
{
    Collider::Collider(Entity& a_owner, const ComponentID& a_id)
        : Component(a_owner, a_id)
	{
		InitVertices(1);
	}

	Collider::Collider(const unsigned int vertices, Entity& a_owner, const ComponentID& a_id)
        : Component(a_owner, a_id)
	{
		InitVertices(vertices);
	}

	Collider::Collider(Entity& a_owner, const ComponentID& a_id, unsigned a_polygons)
        : Component(a_owner,a_id)
	{
		for(unsigned int i = 0; i < a_polygons;i++)
		{
			m_polygons.emplace_back(Polygon());
		}
	}

	Collider::Collider(std::vector<Polygon> polygons, Entity& a_owner, const ComponentID& a_id)
        : Component(a_owner,a_id)
	{
		InitVertices(static_cast<int>(polygons.size()) * 3);
		int i = 0;
		int j = 0;
		for (auto v : m_vertices)
		{
			v = polygons[i].GetVertices()[j];
			++j;
			if (j == 2)
			{
				j = 0;
				++i;
			}
		}
	}

	void Collider::InitVertices(unsigned a_vertices)
	{
		for (unsigned int i = 0; i < a_vertices - 1; i++)
		{
			m_vertices.emplace_back(glm::vec3());
		}
	}

	std::vector<glm::vec3> const& Collider::GetVertices() const
	{
		return m_vertices;
	}

	size_t Collider::GetVerticesCount() const
	{
		return m_vertices.size();
	}

	std::vector<Polygon> const& Collider::GetPolygons() const
	{
		return m_polygons;
	}

	size_t Collider::GetPolygonsCount() const
	{
		return m_polygons.size();
	}

	bool Collider::OverlapsWith(Collider const& a_collider)
	{
		OPTICK_EVENT();
		OPTICK_TAG("Entity Name: ", m_owner->GetName().c_str());
		if (GetVerticesCount() == 1)
		{
			if (a_collider.GetVerticesCount() == 1)
			{
				const auto* c1 = dynamic_cast<const CircleCollider*>(&*this);
				const auto* c2 = dynamic_cast<const CircleCollider*>(&a_collider);
				return DoCirclesIntersect(*c1, *c2);
			}
			if (a_collider.GetPolygonsCount() == 1)
			{
				return CircleIntersectsWithTriangle(m_vertices[0], m_radius, &a_collider.m_polygons[0]);
			}
			if(a_collider.GetPolygonsCount() == 2)
			{

				if (CircleIntersectsWithTriangle(m_vertices[0], m_radius, &a_collider.m_polygons[0]) ||
					CircleIntersectsWithTriangle(m_vertices[0], m_radius, &a_collider.m_polygons[1]))
				{
					return true;
				}
			}
		}
		else if (GetPolygonsCount() == 1)
		{
			if (a_collider.GetVerticesCount() == 1)
			{
				return CircleIntersectsWithTriangle(a_collider.m_vertices[0], a_collider.m_radius, 
                    &dynamic_cast<const PolygonCollider*>(this)->GetPolygons()[0]);

			}
			if (a_collider.GetPolygonsCount() == 1)
			{
				return TriTri2D(&m_polygons[0], &a_collider.m_polygons[0]);
			}
			 if(a_collider.GetPolygonsCount() == 2)
			{
				const bool first = TriTri2D(&m_polygons[0], &a_collider.m_polygons[0]);
				const bool second = TriTri2D(&m_polygons[0], &a_collider.m_polygons[1]);
				if (first||second)
				{
					return true;
				}
			 	return false;
			}
		}
		else if(GetPolygonsCount() == 2)
		{
			if(a_collider.GetVerticesCount() == 1)
			{
				if(CircleIntersectsWithTriangle(a_collider.m_vertices[0], a_collider.m_radius, &m_polygons[0]) || 
					CircleIntersectsWithTriangle(a_collider.m_vertices[0], a_collider.m_radius, &m_polygons[1]))
				{
					return true;
				}
			}
			else if(a_collider.GetPolygonsCount() == 1)
			{
				const bool first = TriTri2D(&m_polygons[0], &a_collider.m_polygons[0]);
				const bool second = TriTri2D(&m_polygons[1], &a_collider.m_polygons[0]);
				if(first || second)
				{
					return true;
				}
				return false;
			}
			else if(a_collider.GetPolygonsCount() == 2)
			{
				if (TriTri2D(&m_polygons[0], &a_collider.m_polygons[0]) ||
					TriTri2D(&m_polygons[1], &a_collider.m_polygons[0]) ||
					TriTri2D(&m_polygons[0], &a_collider.m_polygons[1]) ||
					TriTri2D(&m_polygons[1], &a_collider.m_polygons[1]))
				{
					return true;
				}
			}
		}
		return false;
	}

	void Collider::Render()
	{
	}

	void Collider::SetIsColliding(bool isColliding)
	{
		m_isColliding = isColliding;
	}

	bool const& Collider::GetIsColliding() const
	{
		return m_isColliding;
	}

	void Collider::SetColliderID(int a_newID)
	{
		m_id = a_newID;
	}

	int Collider::GetColliderID() const
	{
		return m_id;
	}

	void Collider::SetTag(const std::string& a_newName)
	{
		m_tag = a_newName;
	}

	const std::string& Collider::GetName() const
	{
		return m_tag;
	}

	Mesh* Collider::GetDrawableShape()
	{
		return nullptr;
	}
}
