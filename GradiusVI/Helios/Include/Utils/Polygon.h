#pragma once
#include "Components/Transform.h"

namespace Helios
{
	struct ps
	{
		std::array<Vec3, 3> m_vertices;
		Vec2 position                           {0.f};
		Vec2 scale                              { 0.f};
		Vec2 origin                             { 0.f };
		float rotation2d                        { 0.f};
	};
	
	using PolygonShape = ps;

	class Polygon
	{
	public:
		glm::vec3 m_vertices[3];

		Polygon();
		Polygon(std::vector<glm::vec3>& a_vertices);

		const PolygonShape& GetPoly() const;
		void SetPosition(float a_x, float a_y);
		void SetRotation(float a_degrees);
		void SetScale(float a_x, float a_y);
		void SetOrigin(float a_x, float a_y);
		void SetVertexPosition(unsigned a_id, glm::vec3 a_position);

		glm::vec3* GetVertices();

        const glm::mat3& GetPolygonShapeMatrix() const { return m_shapeMatrix; }
	private:
		PolygonShape m_polygonShape;

        glm::mat3 m_shapeMatrix { 1.f };
        void CalculateShapeMatrix();
	};
}

