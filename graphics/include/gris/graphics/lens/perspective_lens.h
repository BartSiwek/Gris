#pragma once

#include <glm/glm.hpp>

#include <cmath>

namespace Gris::Graphics::Lens
{

glm::mat4 PerspectiveLensUpdate(float zoom_factor, float aspect_ratio, float w, float h, float n, float f);

class PerspectiveLens
{
public:
    PerspectiveLens()
        : m_projMatrix(1.0F)
        , m_zoomFactor(1.0f)
        , m_near(1.0f)
        , m_far(2.0f)
        , m_frustumWidth(1.0f)
        , m_frustumHeight(1.0f)
    {
    }

    ~PerspectiveLens() = default;

    PerspectiveLens(const PerspectiveLens &) = delete;
    PerspectiveLens & operator=(const PerspectiveLens &) = delete;

    PerspectiveLens(PerspectiveLens &&) = default;
    PerspectiveLens & operator=(PerspectiveLens &&) = default;

    void SetFrustum(float near_plane, float far_plane, float aspect_ratio, float horizontal_fov)
    {
        m_near = near_plane;
        m_far = far_plane;

        auto horizontal_fov_half_tan = std::tan(horizontal_fov / 2.0f);
        m_frustumWidth = 2.0f * m_near * horizontal_fov_half_tan;
        m_frustumHeight = 2.0f * m_near * horizontal_fov_half_tan / aspect_ratio;
    }

    void SetZoomFactor(float zoomFactor)
    {
        m_zoomFactor = zoomFactor;
    }

    float GetZoomFactor() const
    {
        return m_zoomFactor;
    }

    void UpdateMatrices(float aspect_ratio)
    {
        m_projMatrix = PerspectiveLensUpdate(m_zoomFactor, aspect_ratio, m_frustumWidth, m_frustumHeight, m_near, m_far);
    }

    const glm::mat4 & GetProjectionMatrix() const
    {
        return m_projMatrix;
    }

private:
    // Finished product
    glm::mat4 m_projMatrix;

    // Zoom
    float m_zoomFactor;

    // Projection
    float m_near;
    float m_far;
    float m_frustumWidth;
    float m_frustumHeight;
};

}  // namespace Gris::Graphics::Lens
