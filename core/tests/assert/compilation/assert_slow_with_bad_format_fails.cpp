#define GRIS_TU_MODE_SLOW

#include <gris/assert.h>

int main()
{
    GRIS_SLOW_ASSERT_FORMAT(true, "Format {:d}", "I am not a number");
    return 0;
}
