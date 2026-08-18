#include "Material.h"

namespace dae
{
    Material_SolidColor::Material_SolidColor(const ColorRGB& color)
        : m_Color(color) { }

    ColorRGB Material_SolidColor::Shade(const HitRecord& hitRecord,
        const Vector3& l, const Vector3& v)
    {
        return m_Color;
    }

    Material_Lambert::Material_Lambert(const ColorRGB& diffuseColor,
        float diffuseReflectance) : m_DiffuseColor(diffuseColor),
        m_DiffuseReflectance(diffuseReflectance) { }

    ColorRGB Material_Lambert::Shade(const HitRecord& hitRecord,
        const Vector3& l, const Vector3& v)
    {
        return BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor);
    }

    Material_LambertPhong::Material_LambertPhong(const ColorRGB& diffuseColor, float kd,
        float ks, float phongExponent) : m_DiffuseColor(diffuseColor),
        m_DiffuseReflectance(kd), m_SpecularReflectance(ks),
        m_PhongExponent(phongExponent) { }

    ColorRGB Material_LambertPhong::Shade(const HitRecord& hitRecord,
        const Vector3& l, const Vector3& v)
    {
        return BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor) +
            BRDF::Phong(m_SpecularReflectance, m_PhongExponent,
                l, v, hitRecord.normal);
    }

    Material_CookTorrence::Material_CookTorrence(const ColorRGB& albedo,
        float metalness, float roughness) : m_Albedo(albedo),
        m_Metalness(metalness), m_Roughness(roughness) { }

    ColorRGB Material_CookTorrence::Shade(const HitRecord& hitRecord,
        const Vector3& l, const Vector3& v)
    {
        // Calculating F0:

        const ColorRGB dielectricF0 { 0.04f, 0.04f, 0.04f };
        const ColorRGB f0 { ColorRGB::Lerp(dielectricF0, m_Albedo, m_Metalness) };

        // Normal and halfway vectors:

        const Vector3 n { hitRecord.normal };
        const Vector3 h { (l + v).SafeNormalized() };

        // Calculating specular:

        const ColorRGB F {
            BRDF::FresnelFunction_Schlick(h, v, f0) };

        const float D {
            BRDF::NormalDistribution_GGX(n, h, m_Roughness) };

        const float G {
            BRDF::GeometryFunction_Smith(n, v, l, m_Roughness) };

        const ColorRGB specular {
            (F * D * G) / (4.0f * Vector3::Dot(v, n) * Vector3::Dot(l, n))};

        // Calculate diffuse:

        const ColorRGB kd {
            (ColorRGB { 1.0f, 1.0f, 1.0f } - F) * (1.0f - m_Metalness) };

        const ColorRGB diffuse { BRDF::Lambert(kd, m_Albedo) };

        return diffuse + specular;
    }
}