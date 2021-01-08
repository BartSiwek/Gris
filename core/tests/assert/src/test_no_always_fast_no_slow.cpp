#include <catch2/catch.hpp>

#define GRIS_TU_MODE_DISABLE_ALWAYS
#define GRIS_TU_MODE_FAST
#define GRIS_TU_MODE_DISABLE_SLOW

#include <gris/assert.h>

namespace
{

void assert_true_always()
{
    GRIS_ALAWYS_ASSERT(true, "True");
}

void assert_false_always()
{
    GRIS_ALAWYS_ASSERT(false, "False");
}

void assert_true_fast()
{
    GRIS_FAST_ASSERT(true, "True");
}

void assert_false_fast()
{
    GRIS_FAST_ASSERT(false, "False");
}

void assert_true_slow()
{
    GRIS_SLOW_ASSERT(true, "True");
}

void assert_false_slow()
{
    GRIS_SLOW_ASSERT(false, "False");
}

}  // namespace

TEST_CASE("Assert with always disabled, fast and slow disabled", "[assert rules]")
{
    Gris::Assert::SetLoggingCallback(&Gris::Assert::NullLoggingCallback);
    Gris::Assert::SetFailureHandler(&Gris::Assert::ThrowHandler);

    SECTION("Always does not fire on success")
    {
        CHECK_NOTHROW(assert_true_always());
    }

    SECTION("Always does not fire on failure (disabled)")
    {
        CHECK_NOTHROW(assert_false_always());
    }

    SECTION("Fast does not fire on success")
    {
        CHECK_NOTHROW(assert_true_fast());
    }

    SECTION("Fast fires on failure")
    {
        CHECK_THROWS_AS(assert_false_fast(), Gris::Assert::AssertionException);
    }

    SECTION("Slow does not fire on success")
    {
        CHECK_NOTHROW(assert_true_slow());
    }

    SECTION("Slow does not fire on failure (disabled)")
    {
        CHECK_NOTHROW(assert_false_slow());
    }
}
