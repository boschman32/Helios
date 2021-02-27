#include "hepch.h"
#include "Utils/Polygon.h"
#include "gtx/matrix_transform_2d.hpp"

namespace Helios
{
	Polygon::Polygon(std::vector<glm::vec3>& a_vertices)
	{
		for (int i = 0; i < 3; i++)
		{
			m_vertices[i] = a_vertices[i];
			m_polygonShape.m_vertices[i] = m_vertices[i];
        }
	}

	Polygon::Polygon()
	{
		for (int i = 0; i < 3; i++)
		{
			m_vertices[i] = glm::vec3();
			m_polygonShape.m_vertices[i] = m_vertices[i];
		}
	}

	const PolygonShape& Polygon::GetPoly() const
	{
		return m_polygonShape;
	}

	void Polygon::SetPosition(float a_x, float a_y)
	{
        const Vec2 pos = Vec2(a_x, a_y);
        if(pos != m_polygonShape.position)
        {
            m_polygonShape.position = pos;

            CalculateShapeMatrix();
        }
	}

	void Polygon::SetRotation(float a_degrees)
	{
        if(m_polygonShape.rotation2d != a_degrees)
        {
            m_polygonShape.rotation2d = a_degrees;

            CalculateShapeMatrix();
        }
	}

	void Polygon::SetScale(float a_x, float a_y)
	{
        const glm::vec2 scale =  glm::vec2(a_x, a_y);
        if(m_polygonShape.scale != scale)
        {
            m_polygonShape.scale =  scale;

            CalculateShapeMatrix();
        }
	}

	void Polygon::SetOrigin(float a_x, float a_y)
	{
		m_polygonShape.origin = Vec2(a_x, a_y);
	}

	void Polygon::SetVertexPosition(unsigned a_id, glm::vec3 a_position)
	{
		if (a_id < 0 || a_id > 2)
		{
			return;
		}
		m_polygonShape.m_vertices[a_id] = a_position;
	}

	glm::vec3* Polygon::GetVertices()
	{
		return m_vertices;
	}

    void Polygon::CalculateShapeMatrix()
    {
        m_shapeMatrix = glm::translate(glm::mat3(1.f), m_polygonShape.position);
        m_shapeMatrix = glm::rotate(m_shapeMatrix, glm::radians(m_polygonShape.rotation2d));
        m_shapeMatrix = glm::scale(m_shapeMatrix, m_polygonShape.scale);
    }
}
