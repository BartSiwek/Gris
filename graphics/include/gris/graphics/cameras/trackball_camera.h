#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>

namespace Gris::Graphics::Cameras
{

enum class TrackballCameraOperation : uint32_t
{
    None = 0,
    Panning = 1,
    Rotating = 2,
    Zooming = 3,
    MaxOperations = 4
};

void TrackballCameraUpdate(
    TrackballCameraOperation desiredState,
    float rotationSpeed,
    float panningSpeed,
    TrackballCameraOperation * currentState,
    glm::vec2 * prevPoint,
    glm::vec2 * currentPoint,
    glm::vec3 * center,
    glm::quat * rotationQuaternion,
    float * radius,
    glm::mat4 * viewMatrix,
    glm::mat4 * viewMatrixInverseTranspose);

class TrackballCamera
{
public:
    TrackballCamera();

    ~TrackballCamera() = default;

    TrackballCamera(const TrackballCamera &) = delete;
    TrackballCamera & operator=(const TrackballCamera &) = delete;

    TrackballCamera(TrackballCamera &&) = default;
    TrackballCamera & operator=(TrackballCamera &&) = default;

    void SetLocation(const glm::vec3 & location);
    [[nodiscard]] glm::vec3 GetLocation() const;

    void SetRadius(float r);
    [[nodiscard]] float GetRadius() const;

    void SetRotationSpeed(float rotationSpeed);
    [[nodiscard]] float GetRotationSpeed() const;

    void SetPanningSpeed(float panningSpeed);
    [[nodiscard]] float GetPanningSpeed() const;

    void LookAt(const glm::vec3 & from, const glm::vec3 & at);

    void SetDesiredState(TrackballCameraOperation desiredState);

    void SetCurrentPoint(const glm::vec2 & currentPoint);

    void UpdateMatrices();

    [[nodiscard]] const glm::mat4 & GetViewMatrix() const;
    [[nodiscard]] const glm::mat4 & GetViewMatrixInverseTranspose() const;

private:
    // Finished product
    glm::mat4 m_viewMatrix = glm::mat4(1.0F);
    glm::mat4 m_viewMatrixInverseTranspose = glm::mat4(1.0F);

    // View
    glm::vec3 m_center = glm::vec3(0.0F, 0.0F, 0.0F);
    glm::quat m_rotationQuaternion = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);
    float m_radius = 1.0F;

    // State
    TrackballCameraOperation m_currentState = TrackballCameraOperation::None;
    glm::vec2 m_prevPoint = glm::vec2(0.0F, 0.0F);
    glm::vec2 m_currentPoint = glm::vec2(0.0F, 0.0F);

    // Extra state
    TrackballCameraOperation m_desiredState = TrackballCameraOperation::None;

    // Operation speed modifiers
    float m_rotationSpeed = glm::half_pi<float>();
    float m_panningSpeed = 1.0F;
};

}  // namespace Gris::Graphics::Cameras
