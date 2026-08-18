#include <SDL.h>
#include <SDL_surface.h>

#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

#include <execution>

namespace dae
{
    Renderer::Renderer(SDL_Window* pWindow) :
        m_pWindow(pWindow), m_pBuffer(SDL_GetWindowSurface(pWindow))
    {
        //Initialize
        SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
        m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
    }

    void Renderer::Render(Scene* pScene) const
    {
        Camera& camera = pScene->GetCamera();
        auto& materials = pScene->GetMaterials();
        auto& lights = pScene->GetLights();

        std::vector<int> pixels(m_Width * m_Height);
        std::iota(pixels.begin(), pixels.end(), 0);

        std::for_each(std::execution::par, pixels.begin(), pixels.end(), [&](int index)
        {
            int px = index % m_Width;
            int py = index / m_Width;

            ColorRGB finalColor { };

            const Ray viewRay { camera.GetRay(px, py, m_Width, m_Height) };

            HitRecord viewHit { };
            pScene->GetClosestHit(viewRay, viewHit);

            if (viewHit.didHit)
            {
                for (auto& light : lights)
                {
                    const Vector3 toLightOrigin {
                        viewHit.origin + (viewHit.normal * 0.001f) };

                    Vector3 toLight { light.origin - toLightOrigin };

                    const float distToLight { toLight.Normalize() };

                    const float observedArea {
                        Vector3::Dot(viewHit.normal, toLight) };

                    if (observedArea < 0)
                        continue;

                    if (m_ShadowsOn)
                    {
                        const Ray lightRay {
                            toLightOrigin, toLight, 0.0f, distToLight };

                        if (pScene->DoesHit(lightRay))
                            continue;
                    }

                    const ColorRGB radiance {
                        light.intensity * light.color /
                        (distToLight * distToLight) };

                    const ColorRGB brdf {
                        materials[viewHit.materialIndex]->Shade(viewHit,
                            toLight, -viewRay.direction)
                    };

                    switch (m_LightEquationMode)
                    {
                        case LightEquationMode::Full:
                            finalColor += radiance * brdf * observedArea;
                            break;
                        case LightEquationMode::ObservedArea:
                            finalColor += ColorRGB { observedArea, observedArea, observedArea };
                            break;
                        case LightEquationMode::Radiance:
                            finalColor += radiance;
                            break;
                        case LightEquationMode::BRDF:
                            finalColor += brdf;
                            break;
                    }

                    finalColor.MaxToOne();
                }
            }

            finalColor.MaxToOne();

            m_pBufferPixels[index] = SDL_MapRGB(
                m_pBuffer->format,
                static_cast<uint8_t>(finalColor.r * 255),
                static_cast<uint8_t>(finalColor.g * 255),
                static_cast<uint8_t>(finalColor.b * 255));
        });

        //Update SDL Surface
        SDL_UpdateWindowSurface(m_pWindow);
    }

    void Renderer::ToggleShadows(SDL_Scancode key)
    {
        if (key != SDL_SCANCODE_F2)
            return;

        m_ShadowsOn = !m_ShadowsOn;
    }

    void Renderer::ToggleLightEquationMode(SDL_Scancode key)
    {
        if (key != SDL_SCANCODE_F3)
            return;

        switch (m_LightEquationMode)
        {
            case LightEquationMode::Full:
                m_LightEquationMode = LightEquationMode::ObservedArea;
                break;
            case LightEquationMode::ObservedArea:
                m_LightEquationMode = LightEquationMode::Radiance;
                break;
            case LightEquationMode::Radiance:
                m_LightEquationMode = LightEquationMode::BRDF;
                break;
            case LightEquationMode::BRDF:
                m_LightEquationMode = LightEquationMode::Full;
                break;
        }
    }

    bool Renderer::SaveBufferToImage() const
    {
        return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
    }
}