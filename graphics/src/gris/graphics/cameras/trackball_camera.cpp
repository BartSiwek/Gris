#include <gris/graphics/cameras/trackball_camera.h>

#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

#include <array>

namespace
{

glm::vec3 GetPointOnUnitSphere(const glm::vec2 & v)
{
    auto lengthSquared = glm::length2(v);
    auto isInUnitCircle = (lengthSquared <= 1.0F);
    if (isInUnitCircle)
    {
        auto diff = sqrt(1.0F - lengthSquared);
        return glm::vec3(v.x, v.y, diff);
    }
    else
    {
        return glm::vec3(v.x / lengthSquared, v.y / lengthSquared, 0.0F);
    }
}

glm::vec3 GetCurrentTranslationVector(const glm::vec2 & startPoint, const glm::vec2 & endPoint, const glm::vec3 & center, const glm::quat & q, const glm::vec2 & frustumSize)
{
    auto v = endPoint - startPoint;
    auto f = 0.5F * frustumSize;
    auto t = glm::vec3(v * f, 0.0F);

    t = glm::inverse(q) * t;
    t = center - t;

    return t;
}

glm::quat GetCurrentRotationQuaternion(const glm::vec2 & startPoint, const glm::vec2 & endPoint, const glm::quat & rotationQuaterion)
{
    constexpr static glm::vec3 ZERO = glm::vec3(0.0F);

    auto s = GetPointOnUnitSphere(startPoint);
    auto e = GetPointOnUnitSphere(endPoint);

    auto axis = glm::cross(e, s);
    if (axis == ZERO)
    {
        return rotationQuaterion;
    }

    auto angle = glm::acos(glm::dot(e, s));

    auto q = glm::angleAxis(angle, axis);

    q = rotationQuaterion * q;

    return q;
}

float GetCurrentRadius(const glm::vec2 & startPoint, const glm::vec2 & endPoint, float radius)
{
    auto delta = endPoint.y - startPoint.y;
    delta = (delta * delta - 3.0f * delta + 4.0f) / 4.0f;
    return delta * radius;
}

void UpdateViewMatrix(const glm::vec3 & center, const glm::quat & rotationQuaterion, float radius, glm::mat4 * viewMatrix, glm::mat4 * viewMatrixInverseTranspose)
{
    
    auto translation = glm::translate(glm::vec3(0.0f, 0.0f, radius));
    auto translationInvTrans = glm::translate(glm::vec3(0.0f, 0.0f, -radius));

    auto rotation = glm::mat4_cast(rotationQuaterion);

    auto centerTranslation = glm::translate(-center);
    auto centerTranslationInvTrans = glm::transpose(glm::translate(center));

    *viewMatrix = centerTranslation * rotation * translation;
    *viewMatrixInverseTranspose = centerTranslationInvTrans * rotation * translationInvTrans;
}

/* STATE SWITCHING HELPERS */
void EndOperation(Gris::Graphics::Cameras::TrackballCameraOperation * current_state, glm::vec2 * startPoint, glm::vec2 * endPoint)
{
    *current_state = Gris::Graphics::Cameras::TrackballCameraOperation::None;
    startPoint->x = startPoint->y = 0.0f;
    endPoint->x = endPoint->y = 0.0f;
}

/* STATE TRANSITION HANDLERS */
void NullTransition(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * /* startPoint */, glm::vec2 * /* endPoint */, glm::vec3 * /* centerStorage */, glm::quat * /* rotationQuaterionStorage */, float * /* radiusStorage */, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */)
{
}

template<Gris::Graphics::Cameras::TrackballCameraOperation Operation>
void StartOperationTransition(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * startPoint, glm::vec2 * endPoint, glm::vec3 * /* centerStorage */, glm::quat * /* rotationQuaterionStorage */, float * /* radiusStorage */, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * /* radius */)
{
    *currentState = Operation;
    *startPoint = *endPoint;
}

void PanningUpdate(const glm::vec2 & frustumSize, Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * startPoint, glm::vec2 * endPoint, glm::vec3 * /* centerStorage */, glm::quat * /* rotationQuaterionStorage */, float * /* radiusStorage */, glm::vec3 * center, glm::quat * rotationQuaterion, float * /* radius */)
{
    *center = GetCurrentTranslationVector(*startPoint, *endPoint, *center, *rotationQuaterion, frustumSize);
}

void RotatingUpdate(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * startPoint, glm::vec2 * endPoint, glm::vec3 * /* centerStorage */, glm::quat * /* rotationQuaterionStorage */, float * /* radiusStorage */, glm::vec3 * /* center */, glm::quat * rotationQuaterion, float * /* radius */)
{
    *rotationQuaterion = GetCurrentRotationQuaternion(*startPoint, *endPoint, *rotationQuaterion);
}

void ZoomingUpdate(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * /* currentState */, glm::vec2 * startPoint, glm::vec2 * endPoint, glm::vec3 * /* centerStorage */, glm::quat * /* rotationQuaterionStorage */, float * /* radiusStorage */, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * radius)
{
    *radius = GetCurrentRadius(*startPoint, *endPoint, *radius);
}

void EndPanning(const glm::vec2 & frustumSize, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * startPoint, glm::vec2 * endPoint, glm::vec3 * centerStorage, glm::quat * /* rotationQuaterionStorage */, float * /* radiusStorage */, glm::vec3 * center, glm::quat * rotationQuaterion, float * /* radius */)
{
    *center = GetCurrentTranslationVector(*startPoint, *endPoint, *center, *rotationQuaterion, frustumSize);
    *centerStorage = *center;  // TODO: This seems unnecessary
    EndOperation(currentState, startPoint, endPoint);
}

void EndRotating(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * startPoint, glm::vec2 * endPoint, glm::vec3 * /* centerStorage */, glm::quat * rotationQuaterionStorage, float * /* radiusStorage */, glm::vec3 * /* center */, glm::quat * rotationQuaterion, float * /* radius */)
{
    *rotationQuaterion = GetCurrentRotationQuaternion(*startPoint, *endPoint, *rotationQuaterion);
    *rotationQuaterionStorage = *rotationQuaterion;
    EndOperation(currentState, startPoint, endPoint);
}

void EndZooming(const glm::vec2 & /* frustumSize */, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * startPoint, glm::vec2 * endPoint, glm::vec3 * /* centerStorage */, glm::quat * /* rotationQuaterionStorage */, float * radiusStorage, glm::vec3 * /* center */, glm::quat * /* rotationQuaterion */, float * radius)
{
    *radius = GetCurrentRadius(*startPoint, *endPoint, *radius);
    *radiusStorage = *radius;
    EndOperation(currentState, startPoint, endPoint);
}

/* STATE TRANSITION HANDLER TABLE */
using TransitionHandler = void (*)(const glm::vec2 & frustumSize, Gris::Graphics::Cameras::TrackballCameraOperation * currentState, glm::vec2 * startPoint, glm::vec2 * endPoint, glm::vec3 * centerStorage, glm::quat * rotationQuaterionStorage, float * radiusStorage, glm::vec3 * center, glm::quat * rotationQuaterion, float * radius);

using TransitionHandlerTable = std::array<std::array<TransitionHandler, static_cast<size_t>(Gris::Graphics::Cameras::TrackballCameraOperation::MaxOperations)>, static_cast<size_t>(Gris::Graphics::Cameras::TrackballCameraOperation::MaxOperations)>;

constexpr TransitionHandlerTable g_transition_table_ = { {
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

}

void Gris::Graphics::Cameras::TrackballCameraUpdate(TrackballCameraOperation desired_state, float frustum_width, float frustum_height, TrackballCameraOperation * current_state, glm::vec2 * start_point, glm::vec2 * end_point, glm::vec3 * center, glm::quat * rotation_quaterion, float * radius, glm::mat4 * view_matrix, glm::mat4 * view_matrix_inverse_transpose)
{
    auto frustum_size_v = glm::vec4(frustum_width, frustum_height, 0.0f, 0.0f);

    auto center_v = center;
    auto rotation_quaterion_v = rotation_quaterion;
    auto radius_f = *radius;

    auto handler = g_transition_table_[static_cast<uint32_t>(*current_state)][static_cast<uint32_t>(desired_state)];
    (*handler)(frustum_size_v, current_state, start_point, end_point, center, rotation_quaterion, radius, center_v, rotation_quaterion_v, &radius_f);

    UpdateViewMatrix(*center_v, *rotation_quaterion_v, radius_f, view_matrix, view_matrix_inverse_transpose);
}
