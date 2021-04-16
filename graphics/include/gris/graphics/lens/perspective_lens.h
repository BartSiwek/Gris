#pragma once

#include <glm/glm.hpp>

namespace Gris::Graphics::Lens
{

glm::mat4 PerspectiveLensUpdate(float zoomFactor, float aspectRatio, float w, float h, float n, float f);

class PerspectiveLens
{
public:
    PerspectiveLens();

    ~PerspectiveLens() = default;

    PerspectiveLens(const PerspectiveLens &) = delete;
    PerspectiveLens & operator=(const PerspectiveLens &) = delete;

    PerspectiveLens(PerspectiveLens &&) = default;
    PerspectiveLens & operator=(PerspectiveLens &&) = default;

    void SetFrustum(float nearPlane, float farPlane, float aspectRatio, float horizontalFov);

    void SetZoomFactor(float zoomFactor);
    [[nodiscard]] float GetZoomFactor() const;

    void UpdateMatrices(float aspectRatio);

    [[nodiscard]] const glm::mat4 & GetProjectionMatrix() const;

private:
    constexpr static float DEFAULT_ZOOM_FACTOR = 1.0F;
    constexpr static float DEFAULT_NEAR_PLANE = 1.0F;
    constexpr static float DEFAULT_FAR_PLANE = 2.0F;
    constexpr static float DEFAULT_FRUSTUM_WIDTH = 1.0F;
    constexpr static float DEFAULT_FRUSTUM_HEIGHT = 1.0F;

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
