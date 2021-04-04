#include <catch2/catch.hpp>

#include <gris/graphics/cameras/trackball_camera.h>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

namespace
{

template<typename MatrixT>
class GlmMatrixApprox : public Catch::Detail::Approx
{
public:
    explicit GlmMatrixApprox(const MatrixT & matrix)
        : Approx(0.0F)
        , m_matrix(matrix)
    {
    }

    friend bool operator==(const MatrixT & lhs, const GlmMatrixApprox & rhs)
    {
        bool result = true;
        for (int col = 0; col < lhs.length(); ++col)
        {
            for (int row = 0; row < lhs[col].length(); ++row)
            {
                result &= (lhs[col][row] == Approx(rhs.m_matrix[col][row]));
            }
        }
        return result;
    }

    [[nodiscard]] const MatrixT & GetMatrix() const
    {
        return m_matrix;
    }

private:
    MatrixT m_matrix;
};

}  // namespace

namespace Catch
{

template<>
struct StringMaker<glm::vec1>
{
    static std::string convert(glm::vec1 const & value)
    {
        return glm::to_string(value);
    }
};

template<>
struct StringMaker<glm::vec2>
{
    static std::string convert(glm::vec2 const & value)
    {
        return glm::to_string(value);
    }
};

template<>
struct StringMaker<glm::vec3>
{
    static std::string convert(glm::vec3 const & value)
    {
        return glm::to_string(value);
    }
};

template<>
struct StringMaker<glm::vec4>
{
    static std::string convert(glm::vec4 const & value)
    {
        return glm::to_string(value);
    }
};

template<>
struct StringMaker<glm::quat>
{
    static std::string convert(glm::quat const & value)
    {
        return glm::to_string(value);
    }
};

template<>
struct StringMaker<glm::mat4>
{
    static std::string convert(glm::mat4 const & value)
    {
        return glm::to_string(value);
    }
};

template<typename MatrixT>
struct StringMaker<GlmMatrixApprox<MatrixT>>
{
    static std::string convert(GlmMatrixApprox<MatrixT> const & value)
    {
        return glm::to_string(value.GetMatrix());
    }
};

}  // namespace Catch

TEST_CASE("Stationary state", "[trackball camera]")
{
    auto runStationaryCameraUpdateTest = [](Gris::Graphics::Cameras::TrackballCameraOperation currentState,
                                            Gris::Graphics::Cameras::TrackballCameraOperation desiredState,
                                            const glm::vec2 prevPoint,
                                            const glm::vec2 currentPoint,
                                            const glm::vec3 center,
                                            const glm::quat & rotationQuaterion,
                                            const float radius,
                                            const float rotationSpeed,
                                            const float panningSpeed)
    {
        auto inputCurrentState = currentState;
        auto inputPrevPoint = prevPoint;
        auto inputCurrentPoint = currentPoint;
        auto inputCenter = center;
        auto inputRotationQuaterion = rotationQuaterion;
        auto inputRadius = radius;

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            desiredState,
            rotationSpeed,
            panningSpeed,
            &inputCurrentState,
            &inputPrevPoint,
            &inputCurrentPoint,
            &inputCenter,
            &inputRotationQuaterion,
            &inputRadius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(inputCurrentState == currentState);
        CHECK(inputPrevPoint == prevPoint);
        CHECK(inputCurrentPoint == currentPoint);

        CHECK(inputCenter == center);
        CHECK(inputRotationQuaterion == rotationQuaterion);
        CHECK(inputRadius == radius);

        return std::make_tuple(viewMatrix, viewMatrixInverseTranspose);
    };

    SECTION("Simple state")
    {
        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);
        auto radius = 0.0F;

        auto rotationSpeed = glm::half_pi<float>();
        auto panningSpeed = 1.0F;

        auto [viewMatrix, viewMatrixInverseTranspose] = runStationaryCameraUpdateTest(
            currentState,
            desiredState,
            prevPoint,
            currentPoint,
            center,
            rotationQuaterion,
            radius,
            rotationSpeed,
            panningSpeed);

        CHECK(viewMatrix == glm::mat4(1.0));
        CHECK(viewMatrixInverseTranspose == glm::mat4(1.0));
    }

    SECTION("Positive radius")
    {
        constexpr static float RADIUS = 4.0F;

        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);
        auto radius = RADIUS;

        auto rotationSpeed = glm::half_pi<float>();
        auto panningSpeed = 1.0F;

        auto [viewMatrix, viewMatrixInverseTranspose] = runStationaryCameraUpdateTest(
            currentState,
            desiredState,
            prevPoint,
            currentPoint,
            center,
            rotationQuaterion,
            radius,
            rotationSpeed,
            panningSpeed);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, RADIUS)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -RADIUS))));
    }

    SECTION("X Axis Rotation")
    {
        constexpr static glm::quat ROTATION_QUATERNION = glm::quat(0.9238795F, 0.3826834F, 0.0F, 0.0F);
        constexpr static float ROTATION_DEGREES = 45.0F;

        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = ROTATION_QUATERNION;
        auto radius = 0.0F;

        auto rotationSpeed = glm::half_pi<float>();
        auto panningSpeed = 1.0F;

        auto [viewMatrix, viewMatrixInverseTranspose] = runStationaryCameraUpdateTest(
            currentState,
            desiredState,
            prevPoint,
            currentPoint,
            center,
            rotationQuaterion,
            radius,
            rotationSpeed,
            panningSpeed);

        auto expectedViewMatrix = glm::eulerAngleXYZ(glm::radians(ROTATION_DEGREES), glm::radians(0.0F), glm::radians(0.0F));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    SECTION("Y Axis Rotation")
    {
        constexpr static glm::quat ROTATION_QUATERNION = glm::quat(0.953717F, 0.0F, 0.3007058F, 0.0F);
        constexpr static float ROTATION_DEGREES = 35.0F;

        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = ROTATION_QUATERNION;
        auto radius = 0.0F;

        auto rotationSpeed = glm::half_pi<float>();
        auto panningSpeed = 1.0F;

        auto [viewMatrix, viewMatrixInverseTranspose] = runStationaryCameraUpdateTest(
            currentState,
            desiredState,
            prevPoint,
            currentPoint,
            center,
            rotationQuaterion,
            radius,
            rotationSpeed,
            panningSpeed);

        auto expectedViewMatrix = glm::eulerAngleXYZ(glm::radians(0.0F), glm::radians(ROTATION_DEGREES), glm::radians(0.0F));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    SECTION("Z Axis Rotation")
    {
        constexpr static glm::quat ROTATION_QUATERNION = glm::quat(0.8870108F, 0.0F, 0.0F, 0.4617486F);
        constexpr static float ROTATION_DEGREES = 55.0F;

        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = ROTATION_QUATERNION;
        auto radius = 0.0F;

        auto rotationSpeed = glm::half_pi<float>();
        auto panningSpeed = 1.0F;

        auto [viewMatrix, viewMatrixInverseTranspose] = runStationaryCameraUpdateTest(
            currentState,
            desiredState,
            prevPoint,
            currentPoint,
            center,
            rotationQuaterion,
            radius,
            rotationSpeed,
            panningSpeed);

        auto expectedViewMatrix = glm::eulerAngleXYZ(glm::radians(0.0F), glm::radians(0.0F), glm::radians(ROTATION_DEGREES));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    SECTION("Full Rotation")
    {
        constexpr static glm::quat ROTATION_QUATERNION = glm::quat(0.7284268F, 0.4149509F, 0.1954527F, 0.5089286F);
        constexpr static float ROTATION_X_DEGREES = 35.0F;
        constexpr static float ROTATION_Y_DEGREES = 45.0F;
        constexpr static float ROTATION_Z_DEGREES = 55.0F;

        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = ROTATION_QUATERNION;
        auto radius = 0.0F;

        auto rotationSpeed = glm::half_pi<float>();
        auto panningSpeed = 1.0F;

        auto [viewMatrix, viewMatrixInverseTranspose] = runStationaryCameraUpdateTest(
            currentState,
            desiredState,
            prevPoint,
            currentPoint,
            center,
            rotationQuaterion,
            radius,
            rotationSpeed,
            panningSpeed);

        auto expectedViewMatrix = glm::eulerAngleXYZ(glm::radians(ROTATION_X_DEGREES), glm::radians(ROTATION_Y_DEGREES), glm::radians(ROTATION_Z_DEGREES));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    SECTION("Center offset")
    {
        constexpr static glm::vec3 CENTER = glm::vec3(4, 1, 3);

        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = CENTER;
        auto rotationQuaterion = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);
        auto radius = 0.0F;

        auto rotationSpeed = glm::half_pi<float>();
        auto panningSpeed = 1.0F;

        auto [viewMatrix, viewMatrixInverseTranspose] = runStationaryCameraUpdateTest(
            currentState,
            desiredState,
            prevPoint,
            currentPoint,
            center,
            rotationQuaterion,
            radius,
            rotationSpeed,
            panningSpeed);

        CHECK(viewMatrix == glm::translate(-CENTER));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(CENTER)));
    }
}

TEST_CASE("Panning", "[trackball camera]")
{
    constexpr static glm::quat IDENTITY_QUATERNION = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);

    constexpr static float DELTA = 0.25F;

    auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

    auto prevPoint = glm::vec2(0.0F, 0.0F);

    auto center = glm::vec3(0, 0, 0);
    auto rotationQuaterion = IDENTITY_QUATERNION;
    auto radius = 0.0F;

    auto rotationSpeed = glm::half_pi<float>();
    auto panningSpeed = 1.0F;

    {
        constexpr static float CURRENT_DELTA = DELTA;
        constexpr static float TRANSLATION_DELTA = CURRENT_DELTA - DELTA;

        auto currentPoint = glm::vec2(CURRENT_DELTA, 0.0F);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Panning,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Panning);
        CHECK(prevPoint == currentPoint);
        CHECK(currentPoint == glm::vec2(CURRENT_DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(TRANSLATION_DELTA, 0.0F, 0.0F)));
        CHECK(viewMatrixInverseTranspose == glm::translate(glm::vec3(-TRANSLATION_DELTA, 0.0F, 0.0F)));
    }

    {
        constexpr static float CURRENT_DELTA = 2 * DELTA;
        constexpr static float TRANSLATION_DELTA = CURRENT_DELTA - DELTA;

        auto currentPoint = glm::vec2(CURRENT_DELTA, 0.0F);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Panning,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Panning);
        CHECK(prevPoint == glm::vec2(DELTA, 0.0F));
        CHECK(currentPoint == glm::vec2(CURRENT_DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(TRANSLATION_DELTA, 0.0F, 0.0F)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(-TRANSLATION_DELTA, 0.0F, 0.0F))));
    }

    {
        constexpr static float CURRENT_DELTA = 3 * DELTA;
        constexpr static float TRANSLATION_DELTA = CURRENT_DELTA - DELTA;

        auto currentPoint = glm::vec2(CURRENT_DELTA, 0.0F);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Panning,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Panning);
        CHECK(prevPoint == glm::vec2(DELTA, 0.0F));
        CHECK(currentPoint == glm::vec2(CURRENT_DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(TRANSLATION_DELTA, 0.0F, 0.0F)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(-TRANSLATION_DELTA, 0.0F, 0.0F))));
    }

    {
        constexpr static float CURRENT_DELTA = 4 * DELTA;
        constexpr static float TRANSLATION_DELTA = CURRENT_DELTA - DELTA;

        auto currentPoint = glm::vec2(CURRENT_DELTA, 0.0F);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::None,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::None);
        CHECK(prevPoint == glm::vec2(0.0F, 0.0F));
        CHECK(currentPoint == glm::vec2(0.0F, 0.0F));

        CHECK(center == glm::vec3(-TRANSLATION_DELTA, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(TRANSLATION_DELTA, 0.0F, 0.0F)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(-TRANSLATION_DELTA, 0.0F, 0.0F))));
    }
}

TEST_CASE("Rotating", "[trackball camera]")
{
    constexpr static glm::quat IDENTITY_QUATERNION = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);
    constexpr static glm::vec3 ROTATION_AXIS = glm::vec3(0.0F, -1.0F, 0.0F);

    constexpr static float DELTA = 0.25F;

    auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

    auto prevPoint = glm::vec2(0.0F, 0.0F);

    auto center = glm::vec3(0, 0, 0);
    auto rotationQuaterion = IDENTITY_QUATERNION;
    auto radius = 0.0F;

    auto rotationSpeed = glm::half_pi<float>();
    auto panningSpeed = 1.0F;

    {
        constexpr static float CURRENT_DELTA = DELTA;

        auto currentPoint = glm::vec2(CURRENT_DELTA, 0.0F);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Rotating,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Rotating);
        CHECK(prevPoint == currentPoint);
        CHECK(currentPoint == glm::vec2(CURRENT_DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::mat4(1.0));
        CHECK(viewMatrixInverseTranspose == glm::mat4(1.0));
    }

    {
        constexpr static float CURRENT_DELTA = 2.0F * DELTA;
        constexpr static float ROTATION_SPEED_FRACTION = 0.25F;

        auto currentPoint = glm::vec2(CURRENT_DELTA, 0.0F);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Rotating,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Rotating);
        CHECK(prevPoint == glm::vec2(DELTA, 0.0F));
        CHECK(currentPoint == glm::vec2(CURRENT_DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        auto expectedViewMatrix = glm::rotate(ROTATION_SPEED_FRACTION * rotationSpeed, ROTATION_AXIS);
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    {
        constexpr static float CURRENT_DELTA = 3.0F * DELTA;
        constexpr static float ROTATION_SPEED_FRACTION = 0.5F;

        auto currentPoint = glm::vec2(CURRENT_DELTA, 0.0F);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Rotating,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Rotating);
        CHECK(prevPoint == glm::vec2(DELTA, 0.0F));
        CHECK(currentPoint == glm::vec2(CURRENT_DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        auto expectedViewMatrix = glm::rotate(ROTATION_SPEED_FRACTION * rotationSpeed, ROTATION_AXIS);
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    {
        constexpr static float CURRENT_DELTA = 4.0F * DELTA;
        constexpr static float ROTATION_SPEED_FRACTION = 0.75F;

        auto currentPoint = glm::vec2(CURRENT_DELTA, 0.0F);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::None,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::None);
        CHECK(prevPoint == glm::vec2(0.0F, 0.0F));
        CHECK(currentPoint == glm::vec2(0.0F, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == glm::angleAxis(ROTATION_SPEED_FRACTION * rotationSpeed, ROTATION_AXIS));
        CHECK(radius == 0.0F);

        auto expectedViewMatrix = glm::rotate(ROTATION_SPEED_FRACTION * rotationSpeed, ROTATION_AXIS);
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }
}

TEST_CASE("Zooming out", "[trackball camera]")
{
    constexpr static glm::quat IDENTITY_QUATERNION = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);

    constexpr static float DELTA = -0.25F;

    auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

    auto prevPoint = glm::vec2(0.0F, 0.0F);

    auto center = glm::vec3(0, 0, 0);
    auto rotationQuaterion = IDENTITY_QUATERNION;
    auto radius = 1.0F;

    auto rotationSpeed = glm::half_pi<float>();
    auto panningSpeed = 1.0F;

    {
        constexpr static float CURRENT_DELTA = DELTA;
        constexpr static float TRANSLATION_FACTOR = 1.0F;

        auto currentPoint = glm::vec2(0.0F, CURRENT_DELTA);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Zooming,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Zooming);
        CHECK(prevPoint == currentPoint);
        CHECK(currentPoint == glm::vec2(0.0F, CURRENT_DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, TRANSLATION_FACTOR)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -TRANSLATION_FACTOR))));
    }

    {
        constexpr static float CURRENT_DELTA = 2.0F * DELTA;
        constexpr static float TRANSLATION_FACTOR = 233.0F / 256.0F;

        auto currentPoint = glm::vec2(0.0F, CURRENT_DELTA);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Zooming,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Zooming);
        CHECK(prevPoint == glm::vec2(0.0F, DELTA));
        CHECK(currentPoint == glm::vec2(0.0F, CURRENT_DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, TRANSLATION_FACTOR)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -TRANSLATION_FACTOR))));
    }

    {
        constexpr static float CURRENT_DELTA = 3.0F * DELTA;
        constexpr static float TRANSLATION_FACTOR = 53.0F / 64.0F;

        auto currentPoint = glm::vec2(0.0F, CURRENT_DELTA);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Zooming,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Zooming);
        CHECK(prevPoint == glm::vec2(0.0F, DELTA));
        CHECK(currentPoint == glm::vec2(0.0F, CURRENT_DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, TRANSLATION_FACTOR)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -TRANSLATION_FACTOR))));
    }

    {
        constexpr static float CURRENT_DELTA = 4.0F * DELTA;
        constexpr static float TRANSLATION_FACTOR = 193.0F / 256.0F;

        auto currentPoint = glm::vec2(0.0F, CURRENT_DELTA);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::None,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::None);
        CHECK(prevPoint == glm::vec2(0.0F, 0.0F));
        CHECK(currentPoint == glm::vec2(0.0F, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == TRANSLATION_FACTOR);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, TRANSLATION_FACTOR)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -TRANSLATION_FACTOR))));
    }
}

TEST_CASE("Zooming in", "[trackball camera]")
{
    constexpr static glm::quat IDENTITY_QUATERNION = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);

    constexpr static float DELTA = 0.25F;

    auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

    auto prevPoint = glm::vec2(0.0F, 0.0F);

    auto center = glm::vec3(0, 0, 0);
    auto rotationQuaterion = IDENTITY_QUATERNION;
    auto radius = 1.0F;

    auto rotationSpeed = glm::half_pi<float>();
    auto panningSpeed = 1.0F;

    {
        constexpr static float CURRENT_DELTA = DELTA;
        constexpr static float TRANSLATION_FACTOR = 1.0F;

        auto currentPoint = glm::vec2(0.0F, CURRENT_DELTA);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Zooming,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Zooming);
        CHECK(prevPoint == currentPoint);
        CHECK(currentPoint == glm::vec2(0.0F, CURRENT_DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, TRANSLATION_FACTOR)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -TRANSLATION_FACTOR))));
    }

    {
        constexpr static float CURRENT_DELTA = 2.0F * DELTA;
        constexpr static float TRANSLATION_FACTOR = 281.0F / 256.0F;

        auto currentPoint = glm::vec2(0.0F, CURRENT_DELTA);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Zooming,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Zooming);
        CHECK(prevPoint == glm::vec2(0.0F, DELTA));
        CHECK(currentPoint == glm::vec2(0.0F, CURRENT_DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, TRANSLATION_FACTOR)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -TRANSLATION_FACTOR))));
    }

    {
        constexpr static float CURRENT_DELTA = 3.0F * DELTA;
        constexpr static float TRANSLATION_FACTOR = 77.0F / 64.0F;

        auto currentPoint = glm::vec2(0.0F, CURRENT_DELTA);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::Zooming,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::Zooming);
        CHECK(prevPoint == glm::vec2(0.0F, DELTA));
        CHECK(currentPoint == glm::vec2(0.0F, CURRENT_DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, TRANSLATION_FACTOR)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -TRANSLATION_FACTOR))));
    }

    {
        constexpr static float CURRENT_DELTA = 4.0F * DELTA;
        constexpr static float TRANSLATION_FACTOR = 337.0F / 256.0F;

        auto currentPoint = glm::vec2(0.0F, CURRENT_DELTA);

        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverseTranspose;

        Gris::Graphics::Cameras::TrackballCameraUpdate(
            Gris::Graphics::Cameras::TrackballCameraOperation::None,
            rotationSpeed,
            panningSpeed,
            &currentState,
            &prevPoint,
            &currentPoint,
            &center,
            &rotationQuaterion,
            &radius,
            &viewMatrix,
            &viewMatrixInverseTranspose);

        CHECK(currentState == Gris::Graphics::Cameras::TrackballCameraOperation::None);
        CHECK(prevPoint == glm::vec2(0.0F, 0.0F));
        CHECK(currentPoint == glm::vec2(0.0F, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == TRANSLATION_FACTOR);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, TRANSLATION_FACTOR)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -TRANSLATION_FACTOR))));
    }
}