#include "Camera.h"

namespace dae
{
    Camera::Camera(const Vector3& _origin, float _fovAngle) :
        origin { _origin }, fovAngle { _fovAngle } { };

    void Camera::Update(Timer* pTimer)
    {
        // Keyboard input:
        const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
        const bool wKey { static_cast<bool>(pKeyboardState[SDL_SCANCODE_W]) ||
            static_cast<bool>(pKeyboardState[SDL_SCANCODE_UP]) };
        const bool aKey { static_cast<bool>(pKeyboardState[SDL_SCANCODE_A]) ||
            static_cast<bool>(pKeyboardState[SDL_SCANCODE_LEFT]) };
        const bool sKey { static_cast<bool>(pKeyboardState[SDL_SCANCODE_S]) ||
            static_cast<bool>(pKeyboardState[SDL_SCANCODE_DOWN]) };
        const bool dKey { static_cast<bool>(pKeyboardState[SDL_SCANCODE_D]) ||
            static_cast<bool>(pKeyboardState[SDL_SCANCODE_RIGHT]) };
        const bool eKey { static_cast<bool>(pKeyboardState[SDL_SCANCODE_E]) };
        const bool qKey { static_cast<bool>(pKeyboardState[SDL_SCANCODE_Q]) };
        const bool shiftKey {
            static_cast<bool>(pKeyboardState[SDL_SCANCODE_LSHIFT]) };

        Vector3 translationInput { };

        if (dKey && !aKey)
            translationInput.x = 1.0f;
        else if (!dKey && aKey)
            translationInput.x = -1.0f;

        if (eKey && !qKey)
            translationInput.y = 1.0f;
        else if (!eKey && qKey)
            translationInput.y = -1.0f;

        if (wKey && !sKey)
            translationInput.z = 1.0f;
        else if (!wKey && sKey)
            translationInput.z = -1.0f;

        translationInput.SafeNormalize();

        // Mouse input:
        int mouseX {}, mouseY {};
        const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
        const bool lmb { (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0 };
        const bool rmb { (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0 };

        // Controller parameters:
        const float shiftSpeedMult { 5.0f };
        const float deltaTime { pTimer->GetElapsed() };

        const float keyboardSpeed { 5.0f * (shiftKey ? shiftSpeedMult : 1.0f) * deltaTime };
        const float mouseSpeed { 0.01f * (shiftKey ? shiftSpeedMult : 1.0f) };
        const float radiansDelta { 0.005f };

        Vector2 rotation { };
        Vector3 translation { };

        // Control modes:
        if (rmb && !lmb)
        {
            rotation.x = radiansDelta * -mouseY;
            rotation.y = radiansDelta * -mouseX;

            translation = translationInput * keyboardSpeed;
        }
        else if (!rmb && lmb)
        {
            translation.z = -mouseY * mouseSpeed;

            rotation.y = radiansDelta * -mouseX;
        }
        else if (rmb && lmb)
        {
            translation.x = -mouseX * mouseSpeed;
            translation.y = mouseY * mouseSpeed;
        }
        else
        {
            translation = translationInput * keyboardSpeed;
        }

        // Apply rotation:
        totalPitch += rotation.x;
        totalPitch = std::clamp(totalPitch, -PIDIV2, PIDIV2);
        totalYaw += rotation.y;

        forward.x = cosf(totalPitch) * cosf(totalYaw);
        forward.y = sinf(totalPitch);
        forward.z = cosf(totalPitch) * sinf(totalYaw);

        right = Vector3::Cross(Vector3::UnitY, forward).SafeNormalized();
        up = Vector3::Cross(forward, right).SafeNormalized();

        // Apply translation:
        origin += (forward * translation.z) + (right * translation.x) +
            (up * translation.y);
    }

    Ray Camera::GetRay(int px, int py, int width, int height) const
    {
        const float ndcX {
            2.0f * ((static_cast<float>(px) + 0.5f) / static_cast<float>(width)) - 1.0f
        };

        const float ndcY {
            1.0f - 2.0f * ((static_cast<float>(py) + 0.5f) / static_cast<float>(height))
        };

        const float aspectRatio { static_cast<float>(width) / height };
        const float halfVerticalFOV { (fovAngle * (PI / 180.0f)) * 0.5f };
        const float tanVertFOV { tanf(halfVerticalFOV) };
        const float tanHorFOV { tanVertFOV * aspectRatio };

        Vector3 localDir = Vector3 {
            ndcX * tanHorFOV,
            ndcY * tanVertFOV,
            1.0f
        };

        Vector3 worldDir {
            (right * localDir.x) +
            (up * localDir.y) +
            (forward * localDir.z)
        };

        return Ray { origin, worldDir.SafeNormalized() };
    }
}