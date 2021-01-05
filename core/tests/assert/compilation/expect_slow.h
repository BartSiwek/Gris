#ifdef GRIS_ALWAYS_ASSERT_IS_ACTIVE
#error "GRIS_ALWAYS_ASSERT_IS_ACTIVE should not be defined"
#endif

#ifdef GRIS_FAST_ASSERT_IS_ACTIVE
#error "GRIS_FAST_ASSERT_IS_ACTIVE should not be defined"
#endif

#ifndef GRIS_SLOW_ASSERT_IS_ACTIVE
#error "GRIS_SLOW_ASSERT_IS_ACTIVE should be defined"
#endif
