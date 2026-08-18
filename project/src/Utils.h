#pragma once

#include "Math.h"
#include "DataTypes.h"

namespace dae
{
    namespace GeometryUtils
    {
        bool HitTest_Sphere(const Sphere& sphere, const Ray& ray,
            HitRecord& hitRecord, bool ignoreHitRecord = false);

        bool HitTest_Sphere(const Sphere& sphere, const Ray& ray);

        bool HitTest_Plane(const Plane& plane, const Ray& ray,
            HitRecord& hitRecord, bool ignoreHitRecord = false);

        bool HitTest_Plane(const Plane& plane, const Ray& ray);

        bool HitTest_Bounds(const Bounds& bounds, const Ray& ray);

        bool HitTest_Triangle(const Triangle& triangle, const Ray& ray,
            HitRecord& hitRecord, bool ignoreHitRecord = false);

        bool HitTest_Triangle(const Triangle& triangle, const Ray& ray);

        bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray,
            HitRecord& hitRecord, bool ignoreHitRecord = false);

        bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray);
    }

    namespace LightUtils
    {
        //Direction from target to light
        Vector3 GetDirectionToLight(const Light& light, const Vector3& origin);

        ColorRGB GetRadiance(const Light& light, const Vector3& target);
    }

    namespace Utils
    {
        //Just parses vertices and indices
        #pragma warning(push)
        #pragma warning(disable : 4505) //Warning unreferenced local function

        bool ParseOBJ(const std::string& filename,
            std::vector<Vector3>& positions,
            std::vector<Vector3>& normals,
            std::vector<int>& indices);

        #pragma warning(pop)
    }
}