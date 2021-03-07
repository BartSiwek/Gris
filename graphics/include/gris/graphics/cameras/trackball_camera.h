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
    TrackballCameraOperation desired_state,
    float frustum_width,
    float frustum_height,
    TrackballCameraOperation * current_state,
    glm::vec2 * start_point,
    glm::vec2 * end_point,
    glm::vec3 * center,
    glm::quat * rotation_quaterion,
    float * radius,
    glm::mat4 * view_matrix,
    glm::mat4 * view_matrix_inverse_transpose);

class TrackballCamera
{
public:
    TrackballCamera();

    ~TrackballCamera() = default;

    TrackballCamera(const TrackballCamera &) = delete;
    TrackballCamera & operator=(const TrackballCamera &) = delete;

    TrackballCamera(TrackballCamera &&) = default;
    TrackballCamera & operator=(TrackballCamera &&) = default;

    void GetLocation(float * x, float * y, float * z) const;
    void SetLocation(float x, float y, float z);

    float GetRadius() const;
    void SetRadius(float r);

    void LookAt(float from_x, float from_y, float from_z, float at_x, float at_y, float at_z);

    void SetDesiredState(TrackballCameraOperation desired_state);

    void SetEndPoint(const glm::vec2 & p);

    void UpdateMatrices(float frustum_width, float frustum_height);

    const glm::mat4 & GetViewMatrix() const;
    const glm::mat4 & GetViewMatrixInverseTranspose() const;

private:
    // Finished product
    glm::mat4 m_view_matrix;
    glm::mat4 m_view_matrix_inverse_transpose;

    // View
    glm::vec3 m_center;
    glm::quat m_rotation_quaterion;
    float m_radius;

    // State
    TrackballCameraOperation m_current_state;
    glm::vec2 m_start_point;
    glm::vec2 m_end_point;

    // Extra state
    TrackballCameraOperation m_desired_state;
};

}  // namespace Gris::Graphics::Cameras
