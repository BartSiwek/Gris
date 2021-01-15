#define GRIS_TU_MODE_FAST

#include <gris/assert.h>

int main()
{
    GRIS_FAST_ASSERT_FORMAT(true, "Format {:d}", "I am not a number");
    return 0;
}
