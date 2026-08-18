#include "DataTypes.h"
#include "Utils.h"

#include <memory>

namespace dae
{
    Triangle::Triangle(const Vector3& _v0, const Vector3& _v1,
        const Vector3& _v2, const Vector3& _normal)
        : v0 { _v0 }, v1 { _v1 }, v2 { _v2 }, normal { _normal.Normalized() }
    { }

    Triangle::Triangle(const Vector3& _v0, const Vector3& _v1,
        const Vector3& _v2, const Vector3& _normal,
        TriangleCullMode _cullMode, unsigned char _materialIndex)
        : v0 { _v0 }, v1 { _v1 }, v2 { _v2 }, normal { _normal },
        cullMode(_cullMode), materialIndex(_materialIndex)
    { }

    Triangle::Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
        v0 { _v0 }, v1 { _v1 }, v2 { _v2 }
    {
        const Vector3 edgeV0V1 = v1 - v0;
        const Vector3 edgeV0V2 = v2 - v0;
        normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
    }

    TriangleMesh::TriangleMesh(const std::vector<Vector3>& _positions,
        const std::vector<int>& _indices, TriangleCullMode _cullMode = TriangleCullMode::BackFaceCulling)
        : positions(_positions), indices(_indices), cullMode(_cullMode)
    {
        CalculateNormals();

        UpdateTransforms();
    }

    TriangleMesh::TriangleMesh(const std::vector<Vector3>& _positions,
        const std::vector<int>& _indices, const std::vector<Vector3>& _normals,
        TriangleCullMode _cullMode = TriangleCullMode::BackFaceCulling)
        : positions(_positions), indices(_indices), normals(_normals), cullMode(_cullMode)
    {
        UpdateTransforms();
    }

    TriangleMesh::TriangleMesh(const std::string filename,
        TriangleCullMode _cullMode = TriangleCullMode::BackFaceCulling)
        : cullMode(_cullMode)
    {
        Utils::ParseOBJ(filename, positions, normals, indices);

        UpdateTransforms();
    }

    void TriangleMesh::Translate(const Vector3& translation)
    {
        translationTransform = Matrix::CreateTranslation(translation);
    }

    void TriangleMesh::RotateY(float yaw)
    {
        rotationTransform = Matrix::CreateRotationY(yaw);
    }

    void TriangleMesh::Scale(const Vector3& scale)
    {
        scaleTransform = Matrix::CreateScale(scale);
    }

    void TriangleMesh::AppendTriangle(const Triangle& triangle,
        bool ignoreTransformUpdate)
    {
        int startIndex = static_cast<int>(positions.size());

        positions.push_back(triangle.v0);
        positions.push_back(triangle.v1);
        positions.push_back(triangle.v2);

        indices.push_back(startIndex);
        indices.push_back(++startIndex);
        indices.push_back(++startIndex);

        normals.push_back(triangle.normal);

        //Not ideal, but making sure all vertices are updated
        if (!ignoreTransformUpdate)
            UpdateTransforms();
    }

    void TriangleMesh::CalculateNormals()
    {
        normals.reserve(indices.size() / 3);

        for (size_t i { 0 }; i < indices.size(); i += 3)
        {
            Vector3 E1 { positions[indices[i + 1]] - positions[indices[i]] };
            Vector3 E2 { positions[indices[i + 2]] - positions[indices[i]] };

            normals.push_back(Vector3::Cross(E1, E2).Normalized());
        }
    }

    void TriangleMesh::UpdateTransforms()
    {
        const auto finalTransform {
            scaleTransform * rotationTransform * translationTransform };

        transformedPositions.clear();
        transformedNormals.clear();

        transformedPositions.reserve(positions.size());
        transformedNormals.reserve(normals.size());

        for (auto& vertex : positions)
        {
            transformedPositions.push_back(finalTransform.TransformPoint(vertex));
        }

        for (auto& normal : normals)
        {
            transformedNormals.push_back(finalTransform.TransformVector(normal));
        }

        CalculateBounds();
    }

    void TriangleMesh::CalculateBounds()
    {
        if (transformedPositions.empty())
        {
            bounds.min = Vector3 { 0.0f, 0.0f, 0.0f };
            bounds.max = Vector3 { 0.0f, 0.0f, 0.0f };

            return;
        }

        Vector3 min { transformedPositions[0] };
        Vector3 max { transformedPositions[0] };

        for (auto& v : transformedPositions)
        {
            if (v.x < min.x) min.x = v.x;
            if (v.y < min.y) min.y = v.y;
            if (v.z < min.z) min.z = v.z;

            if (v.x > max.x) max.x = v.x;
            if (v.y > max.y) max.y = v.y;
            if (v.z > max.z) max.z = v.z;
        }

        bounds.min = min;
        bounds.max = max;
    }

    Vector3 Ray::GetPoint(float t) const
    {
        return origin + t * direction;
    }
}