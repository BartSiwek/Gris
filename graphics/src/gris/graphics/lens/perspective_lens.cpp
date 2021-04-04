#include <gris/graphics/lens/perspective_lens.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtx/transform.hpp>

glm::mat4 Gris::Graphics::Lens::PerspectiveLensUpdate(float zoomFactor, float aspectRatio, float w, float h, float n, float f)
{
    auto invZoomFactor = 1.0F / zoomFactor;

    auto frustumHeight = 1.0F;
    auto frustumWidth = 1.0F;
    if (aspectRatio >= 1.0F)
    {
        frustumHeight = invZoomFactor * h;
        frustumWidth = aspectRatio * frustumHeight;
    }
    else
    {
        frustumWidth = invZoomFactor * w;
        frustumHeight = frustumWidth / aspectRatio;
    }

    auto fovY = 2 * glm::atan(frustumHeight / (2 * n));
    return glm::perspectiveFovLH(fovY, frustumWidth, frustumHeight, n, f);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Lens::PerspectiveLens::PerspectiveLens()
    : m_projMatrix(1.0F)
    , m_zoomFactor(DEFAULT_ZOOM_FACTOR)
    , m_near(DEFAULT_NEAR_PLANE)
    , m_far(DEFAULT_FAR_PLANE)
    , m_frustumWidth(DEFAULT_FRUSTUM_WIDTH)
    , m_frustumHeight(DEFAULT_FRUSTUM_HEIGHT)
{
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Lens::PerspectiveLens::SetFrustum(float nearPlane, float farPlane, float aspectRatio, float horizontalFov)
{
    m_near = nearPlane;
    m_far = farPlane;

    auto horizontalFovHalfTan = std::tan(horizontalFov / 2);
    m_frustumWidth = 2 * m_near * horizontalFovHalfTan;
    m_frustumHeight = 2 * m_near * horizontalFovHalfTan / aspectRatio;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Lens::PerspectiveLens::SetZoomFactor(float zoomFactor)
{
    m_zoomFactor = zoomFactor;
}

// -------------------------------------------------------------------------------------------------

float Gris::Graphics::Lens::PerspectiveLens::GetZoomFactor() const
{
    return m_zoomFactor;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Lens::PerspectiveLens::UpdateMatrices(float aspectRatio)
{
    m_projMatrix = PerspectiveLensUpdate(m_zoomFactor, aspectRatio, m_frustumWidth, m_frustumHeight, m_near, m_far);
}

// -------------------------------------------------------------------------------------------------

const glm::mat4 & Gris::Graphics::Lens::PerspectiveLens::GetProjectionMatrix() const
{
    return m_projMatrix;
}
