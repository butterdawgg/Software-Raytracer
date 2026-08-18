#pragma once

#include "Math.h"
#include "DataTypes.h"
#include "BRDFs.h"

namespace dae
{
    class Material
    {
        public:

        Material() = default;
        virtual ~Material() = default;

        Material(const Material&) = delete;
        Material(Material&&) noexcept = delete;
        Material& operator=(const Material&) = delete;
        Material& operator=(Material&&) noexcept = delete;

        /*
            \brief Function used to calculate the correct color for the specific material and its parameters
            \param hitRecord current hitrecord
            \param l light direction
            \param v view direction
            \return color
        */
        virtual ColorRGB Shade(const HitRecord& hitRecord = { },
            const Vector3& l = { }, const Vector3& v = { }) = 0;
    };

    class Material_SolidColor final : public Material
    {
        private:

        ColorRGB m_Color { colors::White };

        public:

        Material_SolidColor(const ColorRGB& color);

        ColorRGB Shade(const HitRecord& hitRecord,
            const Vector3& l, const Vector3& v) override;
    };

    class Material_Lambert final : public Material
    {
        private:

        ColorRGB m_DiffuseColor { colors::White };
        float m_DiffuseReflectance { 1.f };

        public:

        Material_Lambert(const ColorRGB& diffuseColor, float diffuseReflectance);

        ColorRGB Shade(const HitRecord& hitRecord = { }, const Vector3& l = { },
            const Vector3& v = { }) override;
    };

    class Material_LambertPhong final : public Material
    {
        private:

        ColorRGB m_DiffuseColor { colors::White };
        float m_DiffuseReflectance { 0.5f };
        float m_SpecularReflectance { 0.5f };
        float m_PhongExponent { 1.f };

        public:

        Material_LambertPhong(const ColorRGB& diffuseColor, float kd,
            float ks, float phongExponent);

        ColorRGB Shade(const HitRecord& hitRecord = { }, const Vector3& l = { },
            const Vector3& v = { }) override;
    };

    class Material_CookTorrence final : public Material
    {
        private:

        ColorRGB m_Albedo { 0.955f, 0.637f, 0.538f }; //Copper
        float m_Metalness { 1.0f };
        float m_Roughness { 0.1f }; // [1.0 > 0.0] >> [ROUGH > SMOOTH]

        public:

        Material_CookTorrence(const ColorRGB& albedo, float metalness, float roughness);

        ColorRGB Shade(const HitRecord& hitRecord = { }, const Vector3& l = { },
            const Vector3& v = { }) override;
    };
}