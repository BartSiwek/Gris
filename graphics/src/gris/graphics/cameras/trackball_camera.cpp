#include <gris/graphics/cameras/trackball_camera.h>

#include <gris/assert.h>
#include <gris/log.h>

#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

#include <array>

namespace
{

glm::vec3 GetPointOnUnitSphere(const glm::vec2 & p)
{
    auto lengthSquared = glm::length2(p);
    auto isInUnitCircle = (lengthSquared <= 1.0F);
    if (isInUnitCircle)
    {
        auto diff = sqrt(1.0F - lengthSquared);
        return glm::vec3(p.x, p.y, diff);
    }
    else
    {
        auto length = sqrt(lengthSquared);
        return glm::vec3(p.x / length, p.y / length, 0.0F);
    }
}

glm::vec3 GetCurrentTranslationVector(const glm::vec2 & prevPoint, const glm::vec2 & currentPoint, const glm::vec3 & center, const glm::quat & q, const glm::vec2 & frustumSize)
{
    auto v = currentPoint - prevPoint;
    auto f = 0.5F * frustumSize;
    auto t = glm::vec3(v * f, 0.0F);

    t = t * glm::inverse(q);
    t = center - t;

    return t;
}

glm::quat GetCurrentRotationQuaternion(const glm::vec2 & prevPoint, const glm::vec2 & currentPoint, const glm::quat & rotationQuaterion)
{
    constexpr static glm::vec3 ZERO = glm::vec3(0.0F);

    auto s = GetPointOnUnitSphere(prevPoint);
    auto e = GetPointOnUnitSphere(currentPoint);

    auto sLength = glm::length(s);
    GRIS_ALWAYS_ASSERT(std::abs(sLength - 1) < 1e-6, "The axis must be normalized");
    auto eLength = glm::length(e);
    GRIS_ALWAYS_ASSERT(std::abs(eLength - 1) < 1e-6, "The axis must be normalized");

    auto axis = glm::cross(e, s);
    auto axisLength = glm::length(axis);
    if (axisLength < 1e-6)
    {
        return rotationQuaterion;
    }

    axis /= axisLength;
    
    auto newAxisLength = glm::length(axis);
    GRIS_ALWAYS_ASSERT(std::abs(newAxisLength - 1) < 1e-6, "The axis must be normalized");

    auto dotProd = glm::dot(e, s);
    auto angle = glm::acos(dotProd);

    auto q = glm::angleAxis(angle, axis);
    q = q * rotationQuaterion;

    return q;
}

float GetCurrentRadius(const glm::vec2 & prevPoint, const glm::vec2 & currentPoint, float radius)
{
    auto delta = currentPoint.y - prevPoint.y;
    GRIS_ALWAYS_ASSERT(-2.0F <= delta && delta <= 2.0F, "Delta must be between -2 and 2");
    delta = (delta * delta + 6.0f * delta + 16.0f) / 16.0f;
    return delta * radius;
}

void UpdateViewMatrix(const glm::vec3 & center, const glm::quat & rotationQuaterion, float radius, glm::mat4 * viewMatrix, glm::mat4 * viewMatrixInverseTranspose)
{
    auto translation = glm::translate(glm::vec3(0.0f, 0.0f, radius));
    auto translationInvTrans = glm::transpose(glm::translate(glm::vec3(0.0f, 0.0f, -radius)));

    auto rotation = glm::mat4_cast(rotationQuaterion);

    auto centerTranslation = glm::translate(-center);
    auto centerTranslationInvTrans = glm::transpose(glm::translate(center));

    *viewMatrix = translation * rotation * centerTranslation;
    *viewMatrixInverseTranspose = translationInvTrans * rotation * centerTranslationInvTrans;
}

/* STATE SWITCHING HELPERS */
void EndOperation(Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint)
{
    *currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
    prevPoint->x = prevPoint->y = 0.0f;
    currentPoint->x = currentPoint->y = 0.0f;
}

/* STATE TRANSITION HANDLERS */
void NullTransition(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * /* prevPoint */, glm::vec2 * /* endPcurrentPointoint */, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, glm::vec3 * /* intermediateCenter */, glm::quat * /* intermediateRotationQuaterion */, float * /* intermediateRadius */)
{
}

template<Gris::Graphics::Cameras::TrackballCameraOperation Operation>
void StartOperationTransition(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, glm::vec3 * /* intermediateCenter */, glm::quat * /* intermediateRotationQuaterion */, float * /* intermediateRadius */)
{
    *currentState = Operation;
    *prevPoint = *currentPoint;
}

void PanningUpdate(const glm::vec2 & frustumSize, Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, glm::vec3 * intermediateCenter, glm::quat * intermediateRotationQuaterion, float * /* intermediateRadius */)
{
    *intermediateCenter = GetCurrentTranslationVector(*prevPoint, *currentPoint, *intermediateCenter, *intermediateRotationQuaterion, frustumSize);
}

void RotatingUpdate(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, glm::vec3 * /* intermediateCenter */, glm::quat * intermediateRotationQuaterion, float * /* intermediateRadius */)
{
    *intermediateRotationQuaterion = GetCurrentRotationQuaternion(*prevPoint, *currentPoint, *intermediateRotationQuaterion);
}

void ZoomingUpdate(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, glm::vec3 * /* intermediateCenter */, glm::quat * /* intermediateRotationQuaterion */, float * intermediateRadius)
{
    *intermediateRadius = GetCurrentRadius(*prevPoint, *currentPoint, *intermediateRadius);
}

void EndPanning(const glm::vec2 & frustumSize, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * center, glm::quat * /* rotationQuaterion */, float * /* radius */, glm::vec3 * intermediateCenter, glm::quat * intermediateRotationQuaterion, float * /* radius */)
{
    *intermediateCenter = GetCurrentTranslationVector(*prevPoint, *currentPoint, *center, *intermediateRotationQuaterion, frustumSize);
    *center = *intermediateCenter;
    EndOperation(currentState, prevPoint, currentPoint);
}

void EndRotating(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * rotationQuaterion, float * /* radius */, glm::vec3 * /* intermediateCenter */, glm::quat * intermediateRotationQuaterion, float * /* intermediateRadius */)
{
    *intermediateRotationQuaterion = GetCurrentRotationQuaternion(*prevPoint, *currentPoint, *rotationQuaterion);
    *rotationQuaterion = *intermediateRotationQuaterion;
    EndOperation(currentState, prevPoint, currentPoint);
}

void EndZooming(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * radius, glm::vec3 * /* intermediateCenter */, glm::quat * /* intermediateRotationQuaterion */, float * intermediateRadius)
{
    *intermediateRadius = GetCurrentRadius(*prevPoint, *currentPoint, *radius);
    *radius = *intermediateRadius;
    EndOperation(currentState, prevPoint, currentPoint);
}

/* STATE TRANSITION HANDLER TABLE */
using TransitionHandler = void (*)(const glm::vec2 & frustumSize, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * center, glm::quat * rotationQuaterion, float * radius, glm::vec3 * intermediateCenter, glm::quat * intermediateRotationQuaterion, float * intermediateRadius);

using TransitionHandlerTable = std::array<std::array<TransitionHandler, static_cast<size_t>(Gris::Graphics::Cameras::TrackballCameraOperation::MaxOperations)>, static_cast<size_t>(Gris::Graphics::Cameras::TrackballCameraOperation::MaxOperations)>;

constexpr TransitionHandlerTable g_transitionTable = { {
    // None
    {
        // None -> None
        NullTransition,
        // None -> Panning
        StartOperationTransition<Gris::Graphics::Cameras::TrackballCameraOperation::Panning>,
        // None -> Rotating
        StartOperationTransition<Gris::Graphics::Cameras::TrackballCameraOperation::Rotating>,
        // None -> Zooming
        StartOperationTransition<Gris::Graphics::Cameras::TrackballCameraOperation::Zooming>,
    },
    // Panning
    {
        // Panning -> None
        EndPanning,
        // Panning -> Panning
        PanningUpdate,
        // Panning -> Rotating
        PanningUpdate,
        // Panning -> Zooming
        PanningUpdate,
    },
    // Rotating
    {
        // Rotating -> None
        EndRotating,
        // Rotating -> Panning
        RotatingUpdate,
        // Rotating -> Rotating
        RotatingUpdate,
        // Rotating -> Zooming
        RotatingUpdate,
    },
    // Zooming
    {
        // Zooming -> None
        EndZooming,
        // Zooming -> Panning
        ZoomingUpdate,
        // Zooming -> Rotating
        ZoomingUpdate,
        // Zooming -> Zooming
        ZoomingUpdate,
    },
} };

}  // namespace

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Cameras::TrackballCameraUpdate(TrackballCameraOperation desiredState, const glm::vec2 & frustumSize, TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * center, glm::quat * rotationQuaterion, float * radius, glm::mat4 * viewMatrix, glm::mat4 * viewMatrixInverseTranspose)
{
    auto intermediateCenter = *center;
    auto intermediateRotationQuaterion = *rotationQuaterion;
    auto intermediateRadius = *radius;

    auto handler = g_transitionTable[static_cast<uint32_t>(*currentState)][static_cast<uint32_t>(desiredState)];
    (*handler)(frustumSize, currentState, prevPoint, currentPoint, center, rotationQuaterion, radius, &intermediateCenter, &intermediateRotationQuaterion, &intermediateRadius);

    UpdateViewMatrix(intermediateCenter, intermediateRotationQuaterion, intermediateRadius, viewMatrix, viewMatrixInverseTranspose);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Cameras::TrackballCamera::TrackballCamera()
    : m_currentState(TrackballCameraOperation::None)
    , m_prevPoint(0.0f, 0.0f)
    , m_currentPoint(0.0f, 0.0f)
    , m_center(0.0f, 0.0f, 0.0f)
    , m_rotationQuaterion(1.0F, 0.0F, 0.0F, 0.0F)
    , m_radius(1.0f)
    , m_viewMatrix(glm::mat4(1.0F))
    , m_viewMatrixInverseTranspose(glm::mat4(1.0F))
    , m_desiredState(TrackballCameraOperation::None)
{
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Cameras::TrackballCamera::SetLocation(const glm::vec3 & location)
{
    m_center = location;
}

// -------------------------------------------------------------------------------------------------

glm::vec3 Gris::Graphics::Cameras::TrackballCamera::GetLocation() const
{
    return m_center;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Cameras::TrackballCamera::SetRadius(float r)
{
    m_radius = r;
}

// -------------------------------------------------------------------------------------------------

float Gris::Graphics::Cameras::TrackballCamera::GetRadius() const
{
    return m_radius;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Cameras::TrackballCamera::LookAt(const glm::vec3 & from, const glm::vec3 & at)
{
    constexpr static auto Z_AXIS = glm::vec3(0, 0, 1);

    auto view = glm::normalize(from - at);
    auto axis = glm::cross(Z_AXIS, view);
    auto angle = glm::acos(glm::dot(Z_AXIS, view));

    m_center = at;
    m_rotationQuaterion = glm::angleAxis(angle, axis);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Cameras::TrackballCamera::SetDesiredState(TrackballCameraOperation desired_state)
{
    m_desiredState = desired_state;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Cameras::TrackballCamera::SetCurrentPoint(const glm::vec2 & currentPoint)
{
    m_currentPoint = currentPoint;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Cameras::TrackballCamera::UpdateMatrices(const glm::vec2 & frustumSize)
{
    TrackballCameraUpdate(m_desiredState, frustumSize, &m_currentState, &m_prevPoint, &m_currentPoint, &m_center, &m_rotationQuaterion, &m_radius, &m_viewMatrix, &m_viewMatrixInverseTranspose);
}

// -------------------------------------------------------------------------------------------------

const glm::mat4 & Gris::Graphics::Cameras::TrackballCamera::GetViewMatrix() const
{
    return m_viewMatrix;
}

// -------------------------------------------------------------------------------------------------

const glm::mat4 & Gris::Graphics::Cameras::TrackballCamera::GetViewMatrixInverseTranspose() const
{
    return m_viewMatrixInverseTranspose;
}
