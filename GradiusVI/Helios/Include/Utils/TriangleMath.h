#pragma once
#include "Components/Colliders/PolygonCollider.h"
#include "Components/Colliders/CircleCollider.h"
#include "Core/Core.h"

inline double Det2D(const Vec2& a_p1, const Vec2& a_p2, const Vec2& a_p3)
{
    return a_p1.x * (a_p2.y - a_p3.y) + a_p2.x * (a_p3.y - a_p1.y) + a_p3.x * (a_p1.y - a_p2.y);
}

inline void CheckTriWinding(const Vec2& a_p1, Vec2& a_p2, Vec2& a_p3, bool a_allowReversed)
{
    const double detTri = Det2D(a_p1, a_p2, a_p3);
    if (detTri < 0.0)
    {
        if (a_allowReversed)
        {
            const Vec2 a = a_p3;
            a_p3 = a_p2;
            a_p2 = a;
        }
    }
}

inline bool BoundaryCollideChk(Vec2& a_p1, Vec2& a_p2, Vec2& a_p3, double a_eps)
{
    return Det2D(a_p1, a_p2, a_p3) < a_eps;
}

inline bool BoundaryDoesntCollideChk(Vec2& a_p1, Vec2& a_p2, Vec2& a_p3, double a_eps)
{
    return Det2D(a_p1, a_p2, a_p3) <= a_eps;
}

inline bool TriTri2D(const Helios::Polygon* a_p1, const Helios::Polygon* a_p2, double a_eps = 0.0, bool a_allowReversed = false, bool a_onBoundary = true)
{
    constexpr int numVertices = 3;
    OPTICK_EVENT();
    Vec2 t1[numVertices], t2[numVertices];
    for (int i = 0; i < numVertices; ++i)
    {
        t1[i] = a_p1->GetPolygonShapeMatrix() * Vec3(a_p1->GetPoly().m_vertices[i].x, a_p1->GetPoly().m_vertices[i].y, 1.f);
        t2[i] = a_p2->GetPolygonShapeMatrix() * Vec3(a_p2->GetPoly().m_vertices[i].x, a_p2->GetPoly().m_vertices[i].y, 1.f);
    }

    //Triangles must be expressed anti-clockwise
    CheckTriWinding(t1[0], t1[1], t1[2], a_allowReversed);
    CheckTriWinding(t2[0], t2[1], t2[2], a_allowReversed);

    bool (*chkEdge)(Vec2&, Vec2&, Vec2&, double) = nullptr;
    if (a_onBoundary) //Points on the boundary are considered as colliding
        chkEdge = BoundaryCollideChk;
    else //Points on the boundary are not considered as colliding
        chkEdge = BoundaryDoesntCollideChk;

    //For edge E of trangle 1,
    for (int i = 0; i < numVertices; ++i)
    {
        const int j = (i + 1) % numVertices;

        //Check all points of triangle 2 lay on the external side of the edge E. If
        //they do, the triangles do not collide.
        if (chkEdge(t1[i], t1[j], t2[0], a_eps) &&
            chkEdge(t1[i], t1[j], t2[1], a_eps) &&
            chkEdge(t1[i], t1[j], t2[2], a_eps))
            return false;
    }

    //For edge E of triangle 2,
    for (int i = 0; i < numVertices; ++i)
    {
        const int j = (i + 1) % numVertices;

        //Check all points of triangle 1 lay on the external side of the edge E. If
        //they do, the triangles do not collide.
        if (chkEdge(t2[i], t2[j], t1[0], a_eps) &&
            chkEdge(t2[i], t2[j], t1[1], a_eps) &&
            chkEdge(t2[i], t2[j], t1[2], a_eps))
            return false;
    }

    //The triangles collide
    return true;
}

inline bool TriTri2D(const Helios::PolygonCollider* a_p1, const Helios::PolygonCollider* a_p2)
{
    return TriTri2D(&a_p1->GetPolygons()[0], &a_p2->GetPolygons()[0]);
}

inline float sign(const Vec2& a_p1, const Vec2& a_p2, const Vec2& a_p3)
{
    return (a_p1.x - a_p3.x) * (a_p2.y - a_p3.y) - (a_p2.x - a_p3.x) * (a_p1.y - a_p3.y);
}

inline bool PointInTriangle(const Vec2 & a_pt, Helios::PolygonCollider * a_poly)
{
    constexpr int numVertices = 3;
    Vec2 v[numVertices];

    for (int i = 0; i < numVertices; ++i)
    {
        v[i] = a_poly->GetPolygons()[0].GetPolygonShapeMatrix() * 
            Vec3(a_poly->GetPolygons()[0].m_vertices[i].x, a_poly->GetPolygons()[0].m_vertices[i].y, 1.f);
    }

    const float d1 = sign(a_pt, v[0], v[1]);
    const float d2 = sign(a_pt, v[1], v[2]);
    const float d3 = sign(a_pt, v[2], v[0]);

    const bool has_neg = (d1 < 0.f) || (d2 < 0.f) || (d3 < 0.f);
    const bool has_pos = (d1 > 0.f) || (d2 > 0.f) || (d3 > 0.f);

    return !(has_neg && has_pos);
}

inline bool CircleIntersectsWithTriangle(const Vec3 & a_circleCenter, float a_radius, const Helios::Polygon * a_poly)
{
    constexpr int numVertices = 3;
    Vec2 v[numVertices];
    const Vec2 a_cc = Vec2(a_circleCenter.x, a_circleCenter.y);
    for (int i = 0; i < numVertices; i++)
    {
        v[i] = a_poly->GetPolygonShapeMatrix() * Vec3(a_poly->GetPoly().m_vertices[i].x, a_poly->GetPoly().m_vertices[i].y, 1.f);
    }

    const auto c1 = a_cc - v[0];

    const auto radiusSqr = a_radius * a_radius;
    const auto c1sqr = c1.x * c1.x + c1.y * c1.y - radiusSqr;

    if (c1sqr <= 0.f)
    {
        return true;
    }

    const auto c2 = a_cc - v[1];

    const auto c2sqr = c2.x * c2.x + c2.y * c2.y - radiusSqr;

    if (c2sqr <= 0.f)
    {
        return true;
    }

    const auto c3 = a_cc - v[2];

    auto c3sqr = radiusSqr;
    c3sqr = c3.x * c3.x + c3.y * c3.y - radiusSqr;

    if (c3sqr <= 0.f)
    {
        return true;
    }


    //Edges
    Vec2 e1, e2, e3;
    e1.x = 0.f;
    e1.y = 0.f;

    e2.x = v[2].x - v[1].x;
    e2.y = v[2].y - v[1].y;

    e3.x = v[1].x - v[2].x;
    e3.y = v[1].y - v[2].y;

    if ((e1.y * c1.x - e1.x * c1.y) >= 0.f && (e2.y * c2.x - e2.x * c2.y) >= 0 && (e3.y * c3.x - e3.x * c3.y) >= 0.f)
    {
        return true;
    }


    ;
    //Circle intersect Edge
    auto k = c1.x * e1.x + c1.y * e1.y;
    float len;

    if (k > 0)
    {
        len = e1.x * e1.x + e1.y * e1.y;

        if (k < len)
        {
            if (c1sqr * len <= k * k)
            {
                return true;
            }
        }
    }

    k = c2.x * e2.x + c2.y * e2.y;

    if (k > 0.f)
    {
        len = e2.x * e2.x + e2.y * e2.y;

        if (k < len)
        {
            if (c2sqr * len <= k * k)
            {
                return true;
            }
        }
    }

    k = c3.x * e3.x + c3.y * e3.y;

    if (k > 0.f)
    {
        len = e3.x * e3.x + e3.y * e3.y;

        if (k < len)
        {
            if (c3sqr * len <= k * k)
            {
                return true;
            }
        }
    }
    return false;
}

inline bool CircleIntersectsWithTriangle(const Vec3 & a_circleCenter, float a_radius, Helios::PolygonCollider * a_poly)
{
    return CircleIntersectsWithTriangle(a_circleCenter, a_radius, &a_poly->GetPolygons()[0]);
}

inline float GetDistanceBetweenVectors(const Vec2 & a_a, const Vec2 & a_b)
{
    return sqrt((a_b.y - a_a.y) * (a_b.y - a_a.y) + (a_b.x - a_a.x) * (a_b.x - a_a.x));
}

inline float GetVectorDistanceSquared(const Vec2 & a, const Vec2 & b)
{
    return (b.y - a.y) * (b.y - a.y) + (b.x - a.x) * (b.x - a.x);
}

inline bool DoCirclesIntersect(const Helios::CircleCollider & a_circleA, const Helios::CircleCollider & a_circleB)
{
    OPTICK_EVENT();
    Vec2 originFirst, originSecond;
    originFirst.x = a_circleA.GetTransform().GetPosition().x - a_circleA.GetRadius();
    originFirst.y = a_circleA.GetTransform().GetPosition().y - a_circleA.GetRadius();

    originSecond.x = a_circleB.GetTransform().GetPosition().x - a_circleB.GetRadius();
    originSecond.y = a_circleB.GetTransform().GetPosition().y - a_circleB.GetRadius();

    const float radiusFirst = a_circleA.GetRadius() * 2;
    const float radiusSecond = a_circleB.GetRadius() * 2;

    return GetVectorDistanceSquared(originFirst, originSecond) <= radiusFirst * radiusFirst
        || GetVectorDistanceSquared(originSecond, originFirst) <= radiusSecond * radiusSecond;
}