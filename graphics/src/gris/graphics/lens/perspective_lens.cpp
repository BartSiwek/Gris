#include <gris/graphics/lens/perspective_lens.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtx/transform.hpp>

glm::mat4 Gris::Graphics::Lens::PerspectiveLensUpdate(float zoomFactor, float aspectRatio, float w, float h, float n, float f)
{
    auto invZoomFactor = 1.0f / zoomFactor;
    if (aspectRatio >= 1.0f)
    {
        auto frustumHeight = invZoomFactor * h;
        auto frustumWidth = aspectRatio * frustumHeight;
        auto fovy = 2.0F * glm::atan(frustumHeight / (2.0F * n));
        return glm::perspectiveFovLH(fovy, frustumWidth, frustumHeight, n, f);
    }
    else
    {
        auto aspectRatioInv = 1.0f / aspectRatio;
        auto frustumWidth = invZoomFactor * w;
        auto frustumHeight = aspectRatioInv * frustumWidth;
        auto fovy = 2.0F * glm::atan(frustumHeight / (2.0F * n));
        return glm::perspectiveFovLH(fovy, frustumWidth, frustumHeight, n, f);
    }
}
