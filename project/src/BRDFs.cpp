#include "BRDFs.h"

#include <stdexcept>

namespace dae
{
    namespace BRDF
    {
        ColorRGB Lambert(float kd, const ColorRGB& cd)
        {
            return cd * (kd / PI);
        }

        ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
        {
            return cd * kd * (1.0f / PI);
        }

        ColorRGB Phong(float ks, float exp, const Vector3& l,
            const Vector3& v, const Vector3& n)
        {
            const float nDotLx2 { 2.0f * Vector3::Dot(n, l) };

            const Vector3 r {
                nDotLx2 * n.x - l.x,
                nDotLx2 * n.y - l.y,
                nDotLx2 * n.z - l.z
                };

            const float cosAlpha { Vector3::Dot(r, v) };
            const float specular { ks * std::pow(cosAlpha, exp) };

            return ColorRGB { specular, specular, specular };
        }

        ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v,
            const ColorRGB& f0)
        {
            const float cosHalfAngle {
                std::clamp(Vector3::Dot(h, v), 0.0f, 1.0f) };

            return f0 + (ColorRGB { 1.0f, 1.0f, 1.0f } - f0) *
                std::pow(1.0f - cosHalfAngle, 5);
        }

        float NormalDistribution_GGX(const Vector3& n, const Vector3& h,
            float roughness)
        {
            const float a { roughness * roughness };
            const float a2 { a * a };
            const float nDotH { std::max(Vector3::Dot(n, h), 0.0f) };
            const float nDotH2 { nDotH * nDotH };

            const float denominator = nDotH2 * (a2 - 1.0f) + 1.0f;
            return a2 / (PI * denominator * denominator + 1e-8f);
        }

        float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v,
            float roughness)
        {
            const float k { std::powf(roughness + 1.0f, 2.0f) * 0.125f };
            const float nDotV { std::clamp(Vector3::Dot(n, v), 0.0f, 1.0f) };

            return nDotV / (nDotV * (1 - k) + k);
        }

        float GeometryFunction_Smith(const Vector3& n, const Vector3& v,
            const Vector3& l, float roughness)
        {
            return GeometryFunction_SchlickGGX(n, v, roughness) *
                GeometryFunction_SchlickGGX(n, l, roughness);
        }
    }
}