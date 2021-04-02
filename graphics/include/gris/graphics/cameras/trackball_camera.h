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
    glm::quat * rotationQuaterion,
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
    glm::vec3 GetLocation() const;

    void SetRadius(float r);
    float GetRadius() const;

    void SetRotationSpeed(float rotationSpeed);
    float GetRotationSpeed() const;

    void SetPanningSpeed(float panningSpeed);
    float GetPanningSpeed() const;

    void LookAt(const glm::vec3 & from, const glm::vec3 & at);

    void SetDesiredState(TrackballCameraOperation desiredState);

    void SetCurrentPoint(const glm::vec2 & currentPoint);

    void UpdateMatrices();

    const glm::mat4 & GetViewMatrix() const;
    const glm::mat4 & GetViewMatrixInverseTranspose() const;

private:
    // Finished product
    glm::mat4 m_viewMatrix;
    glm::mat4 m_viewMatrixInverseTranspose;

    // View
    glm::vec3 m_center;
    glm::quat m_rotationQuaterion;
    float m_radius;

    // State
    TrackballCameraOperation m_currentState;
    glm::vec2 m_prevPoint;
    glm::vec2 m_currentPoint;

    // Extra state
    TrackballCameraOperation m_desiredState;

    // Operation speed modifiers
    float m_rotationSpeed;
    float m_panningSpeed;
};

}  // namespace Gris::Graphics::Cameras
