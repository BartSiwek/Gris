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
    float GetZoomFactor() const;

    void UpdateMatrices(float aspectRatio);

    const glm::mat4 & GetProjectionMatrix() const;

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
