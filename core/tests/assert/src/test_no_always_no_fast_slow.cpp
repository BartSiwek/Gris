#include <catch2/catch.hpp>

#define GRIS_TU_MODE_DISABLE_ALWAYS
#define GRIS_TU_MODE_DISABLE_FAST
#define GRIS_TU_MODE_SLOW

#include <gris/assert.h>

namespace
{

void assert_true_always()
{
    GRIS_ALWAYS_ASSERT(true, "True");
}

void assert_format_true_always()
{
    GRIS_ALWAYS_ASSERT_FORMAT(true, "Format {:d}", 42);
}

void assert_false_always()
{
    GRIS_ALWAYS_ASSERT(false, "False");
}

void assert_format_false_always()
{
    GRIS_ALWAYS_ASSERT_FORMAT(false, "Format {:d}", 42);
}

void assert_true_fast()
{
    GRIS_FAST_ASSERT(true, "True");
}

void assert_format_true_fast()
{
    GRIS_FAST_ASSERT_FORMAT(true, "Format {:d}", 42);
}

void assert_false_fast()
{
    GRIS_FAST_ASSERT(false, "False");
}

void assert_format_false_fast()
{
    GRIS_FAST_ASSERT_FORMAT(false, "Format {:d}", 42);
}

void assert_true_slow()
{
    GRIS_SLOW_ASSERT(true, "True");
}

void assert_format_true_slow()
{
    GRIS_SLOW_ASSERT_FORMAT(true, "Format {:d}", 42);
}

void assert_false_slow()
{
    GRIS_SLOW_ASSERT(false, "False");
}

void assert_format_false_slow()
{
    GRIS_SLOW_ASSERT_FORMAT(false, "Format {:d}", 42);
}

}  // namespace

TEST_CASE("Assert with always disabled, fast disabled and slow", "[assert rules]")
{
    Gris::Assert::SetLoggingCallback(&Gris::Assert::NullLoggingCallback);
    Gris::Assert::SetFailureHandler(&Gris::Assert::ThrowHandler);

    SECTION("Always does not fire on success")
    {
        CHECK_NOTHROW(assert_true_always());
        CHECK_NOTHROW(assert_format_true_always());
    }

    SECTION("Always does not fire on failure (disabled)")
    {
        CHECK_NOTHROW(assert_false_always());
        CHECK_NOTHROW(assert_format_false_always());
    }

    SECTION("Fast does not fire on success")
    {
        CHECK_NOTHROW(assert_true_fast());
        CHECK_NOTHROW(assert_format_true_fast());
    }

    SECTION("Fast does not fire on failure (disabled)")
    {
        CHECK_NOTHROW(assert_false_fast());
        CHECK_NOTHROW(assert_format_false_fast());
    }

    SECTION("Slow does not fire on success")
    {
        CHECK_NOTHROW(assert_true_slow());
        CHECK_NOTHROW(assert_format_true_slow());
    }

    SECTION("Slow fires on failure")
    {
        CHECK_THROWS_AS(assert_false_slow(), Gris::Assert::AssertionException);
        CHECK_THROWS_AS(assert_format_false_slow(), Gris::Assert::AssertionException);
    }
}
