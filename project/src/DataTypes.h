#pragma once

#include <stdexcept>
#include <vector>

#include "Math.h"

namespace dae
{
    struct Sphere final
    {
        Vector3 origin { };
        float radius { };

        unsigned char materialIndex { 0 };
    };

    struct Plane final
    {
        Vector3 origin { };
        Vector3 normal { };

        unsigned char materialIndex { 0 };
    };

    struct Bounds final
    {
        Vector3 min;
        Vector3 max;
    };

    enum class TriangleCullMode
    {
        BackFaceCulling,
        FrontFaceCulling,
        NoCulling
    };

    struct Triangle final
    {
        Vector3 v0 { };
        Vector3 v1 { };
        Vector3 v2 { };

        Vector3 normal { };

        TriangleCullMode cullMode { TriangleCullMode::BackFaceCulling };
        unsigned char materialIndex { };

        Triangle() = default;

        Triangle(const Vector3& _v0, const Vector3& _v1,
                 const Vector3& _v2, const Vector3& _normal);

        Triangle(const Vector3& _v0, const Vector3& _v1,
            const Vector3& _v2, const Vector3& _normal,
            TriangleCullMode _cullMode, unsigned char _materialIndex);

        Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2);
    };

    struct TriangleMesh final
    {
        std::vector<Vector3> positions { };
        std::vector<Vector3> normals { };
        std::vector<int> indices { };
        unsigned char materialIndex { };

        TriangleCullMode cullMode { TriangleCullMode::BackFaceCulling };

        Matrix rotationTransform { };
        Matrix translationTransform { };
        Matrix scaleTransform { };

        std::vector<Vector3> transformedPositions { };
        std::vector<Vector3> transformedNormals { };

        Bounds bounds { };

        TriangleMesh() = default;

        TriangleMesh(const std::vector<Vector3>& _positions,
            const std::vector<int>& _indices, TriangleCullMode _cullMode);

        TriangleMesh(const std::vector<Vector3>& _positions,
            const std::vector<int>& _indices,
            const std::vector<Vector3>& _normals,
            TriangleCullMode _cullMode);

        TriangleMesh(const std::string filename,
            TriangleCullMode _cullMode);

        void Translate(const Vector3& translation);

        void RotateY(float yaw);

        void Scale(const Vector3& scale);

        void AppendTriangle(const Triangle& triangle, bool ignoreTransformUpdate = false);

        void CalculateNormals();

        void UpdateTransforms();

        void CalculateBounds();
    };

    enum class LightType
    {
        Point,
        Directional
    };

    struct Light final
    {
        Vector3 origin { };
        Vector3 direction { };
        ColorRGB color { };
        float intensity { };

        LightType type { };
    };

    struct Ray final
    {
        Vector3 origin { };
        Vector3 direction { };

        float min { 0.0001f };
        float max { FLT_MAX };

        Vector3 GetPoint(float t) const;
    };

    struct HitRecord final
    {
        Vector3 origin { };
        Vector3 normal { };
        float t = FLT_MAX;

        bool didHit { false };
        unsigned char materialIndex { 0 };
    };
}