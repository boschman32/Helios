#pragma once
#include "Core/EntityComponent/Component.h"
#include "Components/Transform.h"
#include "Utils/Polygon.h"
#include "Renderer/Mesh.h"

namespace Helios
{
    enum class EColliderTypes
    {
        NONE,
        CIRCLE,
        QUAD,
        POLYGON
    };
    class Collider : public Component
    {
        RTTR_REGISTRATION_FRIEND
            RTTR_ENABLE()
    public:
        Collider(Entity& a_owner, const ComponentID& a_id);
        Collider(unsigned int a_vertices, Entity& a_owner, const ComponentID& a_id);
        Collider(Entity& a_owner, const ComponentID& a_id, unsigned int a_polygons);
        Collider(std::vector<Polygon> a_polygons, Entity& a_owner, const ComponentID& a_id);

        void InitVertices(unsigned a_vertices);
        std::vector<glm::vec3> const& GetVertices() const;
        size_t GetVerticesCount() const;

        Vec2 const& GetSize() const { return m_size; };
        Vec2 const& GetOffset() const { return m_offset; };

        std::vector<Polygon> const& GetPolygons() const;
        size_t GetPolygonsCount() const;
        virtual bool OverlapsWith(Collider const& a_other);

        void SetVisibility(bool a_isVisible, bool a_directScaling = false)
        {
            m_isVisible = a_isVisible;
            m_useDirectScale = a_directScaling;
        }

        bool GetVisibility() const { return true; }

        virtual void Render();
        void SetIsColliding(bool isColliding);
        bool const& GetIsColliding() const;

        void SetColliderID(int a_newID);
        int GetColliderID() const;
        void SetTag(const std::string& a_newName);
        const std::string& GetName() const;

        virtual EColliderTypes GetColliderType() const { return EColliderTypes::NONE; };

        Vec2 m_origin                               { 0.5f, 0.5f};
        float m_offsetRotation                      { 0.f };
        Vec2 m_offset                               { 0.f};
        bool m_useDirectScale                       { false };

        virtual Mesh* GetDrawableShape();
    protected:

        std::unique_ptr<Mesh> m_mesh                { nullptr };
        float m_radius                              { 0.f };
        std::vector<glm::vec3> m_vertices;
        std::vector<Polygon> m_polygons;
        bool m_isVisible                            { false };
        bool m_isColliding                          { false };
        std::string m_tag                           { "" };
        int m_id                                    { 0 };

        Vec3 m_currentPosition                      { 0.f };
        Vec2 m_size                                 { 0.f };
        Vec2 m_oldSize                              { 0.f };
    };
}