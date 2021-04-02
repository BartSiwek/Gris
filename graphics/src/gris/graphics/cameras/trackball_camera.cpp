#include <gris/graphics/cameras/trackball_camera.h>

#include <gris/assert.h>

#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

#include <array>

namespace
{

glm::vec3 GetCurrentTranslationVector(const glm::vec2 & prevPoint, const glm::vec2 & currentPoint, const glm::vec3 & center, const glm::quat & q, float panningSpeed)
{
    auto t = glm::vec3(panningSpeed * (currentPoint - prevPoint), 0.0F);

    t = glm::inverse(q)  * t;
    t = center - t;

    return t;
}

glm::quat GetCurrentRotationQuaternion(const glm::vec2 & prevPoint, const glm::vec2 & currentPoint, const glm::quat & rotationQuaterion, float rotationSpeed)
{
    constexpr static glm::vec3 RIGHT = glm::vec3(1.0F, 0.0F, 0.0F);
    constexpr static glm::vec3 UP = glm::vec3(0.0F, 1.0F, 0.0F);
    constexpr static glm::vec3 EYE = glm::vec3(0.0F, 0.0F, -1.0F);

    auto moveDirection = currentPoint - prevPoint;
    auto moveAmount = glm::length(moveDirection);

    if (moveAmount < 1e-6)
    {
        return rotationQuaterion;
    }

    auto moveVector = moveDirection.x * RIGHT + moveDirection.y * UP;
    auto rotationAxis = glm::normalize(glm::cross(EYE, moveVector));

    auto q = glm::angleAxis(rotationSpeed * moveAmount, rotationAxis);
    q = q * rotationQuaterion;

    return q;
}

float GetCurrentRadius(const glm::vec2 & prevPoint, const glm::vec2 & currentPoint, float radius)
{
    auto delta = currentPoint.y - prevPoint.y;
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
void NullTransition(Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * /* prevPoint */, glm::vec2 * /* endPcurrentPointoint */, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, float /* rotationSpeed */, float /* panningSpeed */, glm::vec3 * /* intermediateCenter */, glm::quat * /* intermediateRotationQuaterion */, float * /* intermediateRadius */)
{
}

template<Gris::Graphics::Cameras::TrackballCameraOperation Operation>
void StartOperationTransition(Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, float /* rotationSpeed */, float /* panningSpeed */, glm::vec3 * /* intermediateCenter */, glm::quat * /* intermediateRotationQuaterion */, float * /* intermediateRadius */)
{
    *currentState = Operation;
    *prevPoint = *currentPoint;
}

void PanningUpdate(Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, float /* rotationSpeed */, float panningSpeed, glm::vec3 * intermediateCenter, glm::quat * intermediateRotationQuaterion, float * /* intermediateRadius */)
{
    *intermediateCenter = GetCurrentTranslationVector(*prevPoint, *currentPoint, *intermediateCenter, *intermediateRotationQuaterion, panningSpeed);
}

void RotatingUpdate(Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, float rotationSpeed, float /* panningSpeed */, glm::vec3 * /* intermediateCenter */, glm::quat * intermediateRotationQuaterion, float * /* intermediateRadius */)
{
    *intermediateRotationQuaterion = GetCurrentRotationQuaternion(*prevPoint, *currentPoint, *intermediateRotationQuaterion, rotationSpeed);
}

void ZoomingUpdate(Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */, float /* rotationSpeed */, float /* panningSpeed */, glm::vec3 * /* intermediateCenter */, glm::quat * /* intermediateRotationQuaterion */, float * intermediateRadius)
{
    *intermediateRadius = GetCurrentRadius(*prevPoint, *currentPoint, *intermediateRadius);
}

void EndPanning(Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * center, glm::quat * /* rotationQuaterion */, float * /* radius */, float /* rotationSpeed */, float panningSpeed, glm::vec3 * intermediateCenter, glm::quat * intermediateRotationQuaterion, float * /* radius */)
{
    *intermediateCenter = GetCurrentTranslationVector(*prevPoint, *currentPoint, *center, *intermediateRotationQuaterion, panningSpeed);
    *center = *intermediateCenter;
    EndOperation(currentState, prevPoint, currentPoint);
}

void EndRotating(Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * rotationQuaterion, float * /* radius */, float rotationSpeed, float /* panningSpeed */, glm::vec3 * /* intermediateCenter */, glm::quat * intermediateRotationQuaterion, float * /* intermediateRadius */)
{
    *intermediateRotationQuaterion = GetCurrentRotationQuaternion(*prevPoint, *currentPoint, *rotationQuaterion, rotationSpeed);
    *rotationQuaterion = *intermediateRotationQuaterion;
    EndOperation(currentState, prevPoint, currentPoint);
}

void EndZooming(Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * radius, float /* rotationSpeed */, float /* panningSpeed */, glm::vec3 * /* intermediateCenter */, glm::quat * /* intermediateRotationQuaterion */, float * intermediateRadius)
{
    *intermediateRadius = GetCurrentRadius(*prevPoint, *currentPoint, *radius);
    *radius = *intermediateRadius;
    EndOperation(currentState, prevPoint, currentPoint);
}

/* STATE TRANSITION HANDLER TABLE */
using TransitionHandler = void (*)(Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * center, glm::quat * rotationQuaterion, float * radius, float rotationSpeed, float panningSpeed, glm::vec3 * intermediateCenter, glm::quat * intermediateRotationQuaterion, float * intermediateRadius);

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

void Gris::Graphics::Cameras::TrackballCameraUpdate(TrackballCameraOperation desiredState, float rotationSpeed, float panningSpeed, TrackballCameraOperation * currentState, glm::vec2 * prevPoint, glm::vec2 * currentPoint, glm::vec3 * center, glm::quat * rotationQuaterion, float * radius, glm::mat4 * viewMatrix, glm::mat4 * viewMatrixInverseTranspose)
{
    auto intermediateCenter = *center;
    auto intermediateRotationQuaterion = *rotationQuaterion;
    auto intermediateRadius = *radius;

    auto handler = g_transitionTable[static_cast<uint32_t>(*currentState)][static_cast<uint32_t>(desiredState)];
    (*handler)(currentState, prevPoint, currentPoint, center, rotationQuaterion, radius, rotationSpeed, panningSpeed, &intermediateCenter, &intermediateRotationQuaterion, &intermediateRadius);

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
    , m_rotationSpeed(glm::half_pi<float>())
    , m_panningSpeed(1.0F)
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

void Gris::Graphics::Cameras::TrackballCamera::SetRotationSpeed(float rotationSpeed)
{
    m_rotationSpeed = rotationSpeed;
}

// -------------------------------------------------------------------------------------------------

float Gris::Graphics::Cameras::TrackballCamera::GetRotationSpeed() const
{
    return m_rotationSpeed;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Cameras::TrackballCamera::SetPanningSpeed(float panningSpeed)
{
    m_panningSpeed = panningSpeed;
}

// -------------------------------------------------------------------------------------------------

float Gris::Graphics::Cameras::TrackballCamera::GetPanningSpeed() const
{
    return m_panningSpeed;
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

void Gris::Graphics::Cameras::TrackballCamera::UpdateMatrices()
{
    TrackballCameraUpdate(m_desiredState, m_rotationSpeed, m_panningSpeed, &m_currentState, &m_prevPoint, &m_currentPoint, &m_center, &m_rotationQuaterion, &m_radius, &m_viewMatrix, &m_viewMatrixInverseTranspose);
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
