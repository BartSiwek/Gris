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
        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);
        auto radius = 4.0F;

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

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, 4.0F)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -4.0F))));
    }

    SECTION("X Axis Rotation")
    {
        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = glm::quat(0.9238795F, 0.3826834F, 0.0F, 0.0F);
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

        auto expectedViewMatrix = glm::eulerAngleXYZ(glm::radians(45.0F), glm::radians(0.0F), glm::radians(0.0F));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    SECTION("Y Axis Rotation")
    {
        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = glm::quat(0.953717F, 0.0F, 0.3007058F, 0.0F);
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

        auto expectedViewMatrix = glm::eulerAngleXYZ(glm::radians(0.0F), glm::radians(35.0F), glm::radians(0.0F));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    SECTION("Z Axis Rotation")
    {
        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = glm::quat(0.8870108F, 0.0F, 0.0F, 0.4617486F);
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

        auto expectedViewMatrix = glm::eulerAngleXYZ(glm::radians(0.0F), glm::radians(0.0F), glm::radians(55.0F));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    SECTION("Full Rotation")
    {
        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(0, 0, 0);
        auto rotationQuaterion = glm::quat(0.7284268F, 0.4149509F, 0.1954527F, 0.5089286F);
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

        auto expectedViewMatrix = glm::eulerAngleXYZ(glm::radians(35.0F), glm::radians(45.0F), glm::radians(55.0F));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    SECTION("Center offset")
    {
        auto currentState = Gris::Graphics::Cameras::TrackballCameraOperation::None;
        auto desiredState = Gris::Graphics::Cameras::TrackballCameraOperation::None;

        auto prevPoint = glm::vec2(-1.0F, 0.0F);
        auto currentPoint = glm::vec2(1.0F, 0.0F);

        auto center = glm::vec3(4, 1, 3);
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

        CHECK(viewMatrix == glm::translate(glm::vec3(-4, -1, -3)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(4, 1, 3))));
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
        auto currentPoint = glm::vec2(DELTA, 0.0F);

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
        CHECK(currentPoint == glm::vec2(DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::mat4(1.0));
        CHECK(viewMatrixInverseTranspose == glm::mat4(1.0));
    }

    {
        auto currentPoint = glm::vec2(2.0F * DELTA, 0.0F);

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
        CHECK(currentPoint == glm::vec2(2 * DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(DELTA, 0.0F, 0.0F)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(-DELTA, 0.0F, 0.0F))));
    }

    {
        auto currentPoint = glm::vec2(3 * DELTA, 0.0F);

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
        CHECK(currentPoint == glm::vec2(3.0F * DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(2.0F * DELTA, 0.0F, 0.0F)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(-2.0F * DELTA, 0.0F, 0.0F))));
    }

    {
        auto currentPoint = glm::vec2(1.0F, 0.0F);

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

        CHECK(center == glm::vec3(-3.0F * DELTA, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(3.0F * DELTA, 0.0F, 0.0F)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(-3.0F * DELTA, 0.0F, 0.0F))));
    }
}

TEST_CASE("Rotating", "[trackball camera]")
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
        auto currentPoint = glm::vec2(DELTA, 0.0F);

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
        CHECK(currentPoint == glm::vec2(DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        CHECK(viewMatrix == glm::mat4(1.0));
        CHECK(viewMatrixInverseTranspose == glm::mat4(1.0));
    }

    {
        auto currentPoint = glm::vec2(2.0F * DELTA, 0.0F);

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
        CHECK(currentPoint == glm::vec2(2 * DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        auto expectedViewMatrix = glm::rotate(rotationSpeed / 4.0F, glm::vec3(0.0F, -1.0F, 0.0F));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    {
        auto currentPoint = glm::vec2(3 * DELTA, 0.0F);

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
        CHECK(currentPoint == glm::vec2(3.0F * DELTA, 0.0F));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 0.0F);

        auto expectedViewMatrix = glm::rotate(rotationSpeed / 2.0F, glm::vec3(0.0F, -1.0F, 0.0F));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }

    {
        auto currentPoint = glm::vec2(1.0F, 0.0F);

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
        CHECK(rotationQuaterion == glm::angleAxis(3.0F * rotationSpeed / 4.0F, glm::vec3(0.0F, -1.0F, 0.0F)));
        CHECK(radius == 0.0F);

        auto expectedViewMatrix = glm::rotate(3.0F * rotationSpeed / 4.0F, glm::vec3(0.0F, -1.0F, 0.0F));
        CHECK(viewMatrix == GlmMatrixApprox(expectedViewMatrix));
        CHECK(viewMatrixInverseTranspose == GlmMatrixApprox(expectedViewMatrix));
    }
}

TEST_CASE("Zooming out", "[trackball camera]")
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
        auto currentPoint = glm::vec2(0.0F, -DELTA);

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
        CHECK(currentPoint == glm::vec2(0.0F, -DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, 1.0F)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -1.0F))));
    }

    {
        auto currentPoint = glm::vec2(0.0F, -2.0F * DELTA);
        auto factor = 233.0F / 256.0F;

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
        CHECK(prevPoint == glm::vec2(0.0F, -DELTA));
        CHECK(currentPoint == glm::vec2(0.0F, -2.0F * DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, factor)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -factor))));
    }

    {
        auto currentPoint = glm::vec2(0.0F, -3.0F * DELTA);
        auto factor = 53.0F / 64.0F;

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
        CHECK(prevPoint == glm::vec2(0.0F, -DELTA));
        CHECK(currentPoint == glm::vec2(0.0F, -3.0F * DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, factor)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -factor))));
    }

    {
        auto currentPoint = glm::vec2(0.0F, -4.0F * DELTA);
        auto factor = 193.0F / 256.0F;

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
        CHECK(radius == factor);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, factor)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -factor))));
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
        auto currentPoint = glm::vec2(0.0F, DELTA);

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
        CHECK(currentPoint == glm::vec2(0.0F, DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, 1.0F)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -1.0F))));
    }

    {
        auto currentPoint = glm::vec2(0.0F, 2.0F * DELTA);
        auto factor = 281.0F / 256.0F;

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
        CHECK(currentPoint == glm::vec2(0.0F, 2.0F * DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, factor)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -factor))));
    }

    {
        auto currentPoint = glm::vec2(0.0F, 3.0F * DELTA);
        auto factor = 77.0F / 64.0F;

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
        CHECK(currentPoint == glm::vec2(0.0F, 3.0F * DELTA));

        CHECK(center == glm::vec3(0, 0, 0));
        CHECK(rotationQuaterion == IDENTITY_QUATERNION);
        CHECK(radius == 1.0F);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, factor)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -factor))));
    }

    {
        auto currentPoint = glm::vec2(0.0F, 4.0F * DELTA);
        auto factor = 337.0F / 256.0F;

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
        CHECK(radius == factor);

        CHECK(viewMatrix == glm::translate(glm::vec3(0.0F, 0.0F, factor)));
        CHECK(viewMatrixInverseTranspose == glm::transpose(glm::translate(glm::vec3(0.0F, 0.0F, -factor))));
    }
}