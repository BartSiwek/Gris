#define GRIS_TU_MODE_ALWAYS

#include <gris/assert.h>

int main()
{
    GRIS_ALWAYS_ASSERT_FORMAT(true, "Format {:d}", "I am not a number");
    return 0;
}
