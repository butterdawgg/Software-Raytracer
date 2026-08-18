#pragma once

#include <cstdint>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
    class Scene;

    enum class LightEquationMode
    {
        Full,
        ObservedArea,
        Radiance,
        BRDF
    };

    class Renderer final
    {
        private:

        SDL_Window* m_pWindow {};

        SDL_Surface* m_pBuffer {};
        uint32_t* m_pBufferPixels {};

        int m_Width {};
        int m_Height {};

        bool m_ShadowsOn { true };
        LightEquationMode m_LightEquationMode { LightEquationMode::Full };

        public:

        Renderer(SDL_Window* pWindow);
        ~Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) noexcept = delete;
        Renderer& operator=(Renderer&&) noexcept = delete;

        void Render(Scene* pScene) const;
        void ToggleShadows(SDL_Scancode key);
        void ToggleLightEquationMode(SDL_Scancode key);
        bool SaveBufferToImage() const;
    };
}
