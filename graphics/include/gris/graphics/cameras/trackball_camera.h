#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201)
#endif

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

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
    TrackballCamera()
        : m_current_state(TrackballCameraOperation::None)
        , m_start_point(0.0f, 0.0f)
        , m_end_point(0.0f, 0.0f)
        , m_center(0.0f, 0.0f, 0.0f)
        , m_rotation_quaterion()
        , m_radius(1.0f)
        , m_view_matrix(glm::mat4(1.0F))
        , m_view_matrix_inverse_transpose(glm::mat4(1.0F))
        , m_desired_state(TrackballCameraOperation::None)
    {
    }

    ~TrackballCamera() = default;

    TrackballCamera(const TrackballCamera &) = delete;
    TrackballCamera & operator=(const TrackballCamera &) = delete;

    TrackballCamera(TrackballCamera &&) = default;
    TrackballCamera & operator=(TrackballCamera &&) = default;

    void GetLocation(float * x, float * y, float * z) const
    {
        *x = m_center.x;
        *y = m_center.y;
        *z = m_center.z;
    }

    void SetLocation(float x, float y, float z)
    {
        m_center.x = x;
        m_center.y = y;
        m_center.z = z;
    }

    float GetRadius() const
    {
        return m_radius;
    }

    void SetRadius(float r)
    {
        m_radius = r;
    }

    void LookAt(float from_x, float from_y, float from_z, float at_x, float at_y, float at_z)
    {
        constexpr static auto Z_AXIS = glm::vec3(0, 0, 1);

        m_center.x = at_x;
        m_center.y = at_y;
        m_center.z = at_z;

        auto from = glm::vec3(from_x, from_y, from_z);
        auto view = glm::normalize(from - m_center);

        auto axis = glm::cross(Z_AXIS, view);
        auto angle = glm::acos(glm::dot(Z_AXIS, view));

        m_rotation_quaterion = glm::angleAxis(angle, axis);
    }

    void SetDesiredState(TrackballCameraOperation desired_state)
    {
        m_desired_state = desired_state;
    }

    void SetEndPoint(const glm::vec2 & p)
    {
        m_end_point = p;
    }

    void UpdateMatrices(float frustum_width, float frustum_height)
    {
        TrackballCameraUpdate(m_desired_state, frustum_width, frustum_height, &m_current_state, &m_start_point, &m_end_point, &m_center, &m_rotation_quaterion, &m_radius, &m_view_matrix, &m_view_matrix_inverse_transpose);
    }

    const glm::mat4 & GetViewMatrix() const
    {
        return m_view_matrix;
    }

    const glm::mat4 & GetViewMatrixInverseTranspose() const
    {
        return m_view_matrix_inverse_transpose;
    }

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
