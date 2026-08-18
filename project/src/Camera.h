#pragma once

#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"
#include "DataTypes.h"

namespace dae
{
    constexpr float PIDIV2 { 1.5707963f };

    struct Camera final
    {
        Vector3 origin { };
        float fovAngle { 90.0f };

        Vector3 forward { Vector3::UnitZ };
        Vector3 up { Vector3::UnitY };
        Vector3 right { Vector3::UnitX };

        float totalPitch { 0.f };
        float totalYaw { PIDIV2 };

        Camera() = default;
        Camera(const Vector3& _origin, float _fovAngle);

        void Update(Timer* pTimer);

        Ray GetRay(int px, int py, int width, int height) const;
    };
}