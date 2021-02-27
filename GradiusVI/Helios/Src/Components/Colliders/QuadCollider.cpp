#include "hepch.h"
#include "Components/Colliders/QuadCollider.h"
#include "Core/EntityComponent/Entity.h"

namespace Helios
{
    QuadCollider::QuadCollider(Entity& a_owner, const ComponentID& a_id)
    : Collider(a_owner, a_id, 2)
    {
		SetSize(32, 32);
		m_mesh = Mesh::CreateCube();
    }

    void QuadCollider::SetSize(float a_x, float a_y)
    {
        m_polygons[0].SetVertexPosition(0, glm::vec3());
        m_polygons[0].SetVertexPosition(1, glm::vec3(a_x, 0, 0));
        m_polygons[0].SetVertexPosition(2, glm::vec3(a_x, a_y, 0));

        m_polygons[0].SetOrigin(m_origin.x * a_x, m_origin.y * a_y);

        m_polygons[1].SetVertexPosition(0, glm::vec3());
        m_polygons[1].SetVertexPosition(1, glm::vec3(0, a_y, 0));
        m_polygons[1].SetVertexPosition(2, glm::vec3(a_x, a_y, 0));

        m_polygons[1].SetOrigin(m_origin.x * a_x, m_origin.y * a_y);

        m_size.x = a_x;
        m_size.y = a_y;

        m_oldSize = m_size;
    }

    void QuadCollider::Render()
    {
        OPTICK_EVENT()

        m_polygons[0].SetPosition(GetTransform().GetPosition().x + m_offset.x, GetTransform().GetPosition().y + m_offset.y);
        m_polygons[0].SetRotation(GetTransform().GetRotation().z + m_offsetRotation);
        m_polygons[0].SetScale(GetTransform().GetScale().x, GetTransform().GetScale().y);

        m_polygons[1].SetPosition(GetTransform().GetPosition().x + m_offset.x, GetTransform().GetPosition().y + m_offset.y);
        m_polygons[1].SetRotation(GetTransform().GetRotation().z + m_offsetRotation);
        m_polygons[1].SetScale(GetTransform().GetScale().x, GetTransform().GetScale().y);

        if (m_oldSize != m_size)
        {
            SetSize(m_size.x, m_size.y);
        }

        m_currentPosition = GetTransform().GetPosition();
    }

    Mesh* QuadCollider::GetDrawableShape()
    {
		return m_mesh.get();
    }
}
