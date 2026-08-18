#include "Utils.h"

#include <fstream>

namespace dae
{
    namespace GeometryUtils
    {
        constexpr float EPS { 1e-6f };

        bool HitTest_Sphere(const Sphere& sphere, const Ray& ray,
            HitRecord& hitRecord, bool ignoreHitRecord)
        {
            const Vector3 sphereToRay { ray.origin - sphere.origin };
            const float a { Vector3::Dot(ray.direction, ray.direction) };
            const float b { Vector3::Dot(sphereToRay, ray.direction) * 2.0f };
            const float c { Vector3::Dot(sphereToRay, sphereToRay) -
                sphere.radius * sphere.radius };

            const float D { (b * b) - (4.0f * a * c) };

            if (D < 0.0f)
                return false;

            const float sqrtD { sqrtf(D) };
            const float inv2a { 0.5f / a };

            const float t1 { (-b - sqrtD) * inv2a };
            const float t2 { (-b + sqrtD) * inv2a };

            float t { std::min(t1, t2) };

            if (t < ray.min || t > ray.max)
                return false;

            if (ignoreHitRecord)
                return true;

            hitRecord.origin = ray.GetPoint(t);
            hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
            hitRecord.t = t;
            hitRecord.didHit = true;
            hitRecord.materialIndex = sphere.materialIndex;

            return true;
        }

        bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
        {
            HitRecord temp { };
            return HitTest_Sphere(sphere, ray, temp, true);
        }

        bool HitTest_Plane(const Plane& plane, const Ray& ray,
            HitRecord& hitRecord, bool ignoreHitRecord)
        {
            const Vector3 rayToPlane { plane.origin - ray.origin };

            const float normalDotDir {
                Vector3::Dot(plane.normal, ray.direction) };

            if (normalDotDir >= 0.0f)
                return false;

            const float t { Vector3::Dot(plane.normal, rayToPlane)
                / normalDotDir };

            if (t < ray.min || t > ray.max)
                return false;

            if (ignoreHitRecord)
                return true;

            hitRecord.origin = ray.GetPoint(t);
            hitRecord.normal = plane.normal;
            hitRecord.t = t;
            hitRecord.didHit = true;
            hitRecord.materialIndex = plane.materialIndex;

            return true;
        }

        bool HitTest_Plane(const Plane& plane, const Ray& ray)
        {
            HitRecord temp { };
            return HitTest_Plane(plane, ray, temp, true);
        }

        bool HitTest_Bounds(const Bounds& bounds, const Ray& ray)
        {
            // Avoid division by zero by replacing 0 with a very small value
            const float xInvDir { (ray.direction.x != 0.0f) ? 1.0f / ray.direction.x
                : std::numeric_limits<float>::infinity() };

            const float yInvDir { (ray.direction.y != 0.0f) ? 1.0f / ray.direction.y
                : std::numeric_limits<float>::infinity() };

            const float zInvDir { (ray.direction.z != 0.0f) ? 1.0f / ray.direction.z
                : std::numeric_limits<float>::infinity() };

            // Compute the entry and exit points for the x slab
            const float tx1 { (bounds.min.x - ray.origin.x) * xInvDir };
            const float tx2 { (bounds.max.x - ray.origin.x) * xInvDir };

            // Compute the entry and exit points for the y slab
            const float ty1 { (bounds.min.y - ray.origin.y) * yInvDir };
            const float ty2 { (bounds.max.y - ray.origin.y) * yInvDir };

            // Compute the entry and exit points for the z slab
            const float tz1 { (bounds.min.z - ray.origin.z) * zInvDir };
            const float tz2 { (bounds.max.z - ray.origin.z) * zInvDir };

            // Compute entry and exit distances
            const float tMin {
                std::max(std::max(std::min(tx1, tx2), std::min(ty1, ty2)),
                    std::min(tz1, tz2)) };

            const float tMax {
                std::min(std::min(std::max(tx1, tx2), std::max(ty1, ty2)),
                    std::max(tz1, tz2)) };

            if (tMax < tMin)
                return false;

            if (tMax < ray.min)
                return false;

            if (tMin > ray.max)
                return false;

            return true;
        }

        bool HitTest_Triangle(const Triangle& triangle, const Ray& ray,
            HitRecord& hitRecord, bool ignoreHitRecord)
        {
            const Vector3 T { ray.origin - triangle.v0 };
            const Vector3 E1 { triangle.v1 - triangle.v0 };
            const Vector3 E2 { triangle.v2 - triangle.v0 };
            const Vector3 D { ray.direction };

            const Vector3 P { Vector3::Cross(D, E2) };
            const float det { Vector3::Dot(P, E1) };

            TriangleCullMode cullMode { triangle.cullMode };

            if (ignoreHitRecord)
            {
                switch (triangle.cullMode)
                {
                    case TriangleCullMode::FrontFaceCulling:
                        cullMode = TriangleCullMode::BackFaceCulling;
                        break;
                    case TriangleCullMode::BackFaceCulling:
                        cullMode = TriangleCullMode::FrontFaceCulling;
                        break;
                    default:
                        break;
                }
            }

            switch (cullMode)
            {
                case TriangleCullMode::FrontFaceCulling:
                    if (det > -EPS)
                        return false;
                    break;
                case TriangleCullMode::BackFaceCulling:
                    if (det < EPS)
                        return false;
                    break;
                default:
                    if (fabs(det) < EPS)
                        return false;
                    break;
            }

            const Vector3 Q { Vector3::Cross(T, E1) };
            const float denominator { 1.0f / det };

            const float t { Vector3::Dot(Q, E2) * denominator };
            const float u { Vector3::Dot(P, T) * denominator };
            const float v { Vector3::Dot(Q, D) * denominator };

            if (t < ray.min || t > ray.max || u < 0 || v < 0 || u + v > 1)
                return false;

            if (ignoreHitRecord)
                return true;

            hitRecord.didHit = true;
            hitRecord.t = t;
            hitRecord.origin = ray.GetPoint(t);
            hitRecord.normal = triangle.normal;
            hitRecord.materialIndex = triangle.materialIndex;

            if (Vector3::Dot(hitRecord.normal, ray.direction) > 0)
                hitRecord.normal = -hitRecord.normal;

            return true;
        }

        bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
        {
            HitRecord temp { };
            return HitTest_Triangle(triangle, ray, temp, true);
        }

        bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray,
            HitRecord& hitRecord, bool ignoreHitRecord)
        {
            if (!HitTest_Bounds(mesh.bounds, ray))
                return false;

            HitRecord closestHit { };

            for (size_t i { 0 }, k { 0 }; i < mesh.indices.size(); i += 3, k++)
            {
                const Triangle tri {
                    mesh.transformedPositions[mesh.indices[i]],
                    mesh.transformedPositions[mesh.indices[i + 1]],
                    mesh.transformedPositions[mesh.indices[i + 2]],
                    mesh.transformedNormals[k],
                    mesh.cullMode,
                    mesh.materialIndex };

                HitRecord temp { };

                const bool hit {
                    HitTest_Triangle(tri, ray, temp, ignoreHitRecord) };

                if (!hit)
                    continue;

                if (ignoreHitRecord)
                    return true;

                if (temp.t >= closestHit.t)
                    continue;

                closestHit = temp;
            }

            if (closestHit.didHit)
            {
                hitRecord = closestHit;
                return true;
            }

            return false;
        }

        bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
        {
            HitRecord temp { };
            return HitTest_TriangleMesh(mesh, ray, temp, true);
        }
    }

    namespace LightUtils
    {
        Vector3 GetDirectionToLight(const Light& light, const Vector3& origin)
        {
            return (light.origin - origin).SafeNormalized();
        }

        ColorRGB GetRadiance(const Light& light, const Vector3& target)
        {
            const float dist { (light.origin - target).Magnitude() };

            return light.intensity * light.color / (dist * dist);
        }
    }

    namespace Utils
    {
        bool ParseOBJ(const std::string& filename,
            std::vector<Vector3>& positions,
            std::vector<Vector3>& normals,
            std::vector<int>& indices)
        {
            std::ifstream file(filename);
            if (!file)
                return false;

            std::string sCommand;

            // start a while iteration ending when the end of
            // file is reached (ios::eof)
            while (!file.eof())
            {
                //read the first word of the string, use the >>
                // operator (istream::operator>>)
                file >> sCommand;

                //use conditional statements to process the different commands
                if (sCommand == "#")
                {
                    // Ignore Comment
                }
                else if (sCommand == "v")
                {
                    //Vertex
                    float x, y, z;
                    file >> x >> y >> z;
                    positions.push_back({ x, y, z });
                }
                else if (sCommand == "f")
                {
                    int i0, i1, i2;
                    file >> i0 >> i1 >> i2;

                    indices.push_back(i0 - 1);
                    indices.push_back(i1 - 1);
                    indices.push_back(i2 - 1);
                }

                //read till end of line and ignore all remaining chars
                file.ignore(1000, '\n');

                if (file.eof())
                    break;
            }

            //Precompute normals
            for (uint64_t index = 0; index < indices.size(); index += 3)
            {
                uint32_t i0 = indices[index];
                uint32_t i1 = indices[index + 1];
                uint32_t i2 = indices[index + 2];

                Vector3 edgeV0V1 = positions[i1] - positions[i0];
                Vector3 edgeV0V2 = positions[i2] - positions[i0];
                Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

                normal.Normalize();

                normals.push_back(normal);
            }

            return true;
        }
    }
}