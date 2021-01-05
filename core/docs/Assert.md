# Gris.Assert
Provides a set of preprocessor assertions macros, GRIS_ALAWYS_ASSERT, GRIS_FAST_ASSERT, GRIS_SLOW_ASSERT with the purpose of providing redundant runtime checks. If an assertion argument evaluates to false, a configurable callback function is called with the current filename, line number, and a nul-terminated message.

Inspired by [BDE 3.0: bsls_assert](https://bloomberg.github.io/bde/group__bsls__assert.html)

# Assert Macros
- GRIS_ALAWYS_ASSERT
- GRIS_FAST_ASSERT
- GRIS_SLOW_ASSERT

# Semantics
- Redundant - no observable difference in program behavior apart from performance
- No side-effects - asserted expressions should not have side effects
- No impact on ABI compatibility - except for when GRIS_SLOW_ASSERT macro is enabled

The macros differ in their intended purpose, object file ABI compatibility and performance impact:

| Macro              | Performance impact          | ABI compatibility between translation units | Purpose                 |
| ------------------ | --------------------------- | ------------------------------------------- | ----------------------- |
| GRIS_ALAWYS_ASSERT | Negligible                  | Required                                    | Always performed checks |
| GRIS_FAST_ASSERT   | Max. 5-10% overhead         | Required                                    | Debug checks            |
| GRIS_SLOW_ASSERT   | Order of magnitude overhead | Optional                                    | Troubleshooting checks  |

The ABI compatibility is related to the target-level modes and tu modes [FIXME]

# Target-level modes
The following macros can be used by the build system/development environment to specify target-wide assert semantics. If more than one is is defined an compile-time error is reported.
- GRIS_TARGET_MODE_NONE - none of the assert macros are active. Used mostly to verify assertions have no observable side-effects.
- GRIS_TARGET_MODE_RELEASE - default build mode. Only GRIS_ALAWYS_ASSERT is active.
- GRIS_TARGET_MODE_DEBUG - GRIS_FAST_ASSERT and GRIS_ALAWYS_ASSERT are active.
- GRIS_TARGET_MODE_SAFE - GRIS_SLOW_ASSERT, GRIS_FAST_ASSERT and GRIS_ALAWYS_ASSERT are active.

# Translation unit-level modes
The following macros can be used to override target-level modes on a per-translation-unit basis. If GRIS_TU_MODE_NONE is defined defining any other translation unit-level macros will result in a compile-time error. GRIS_TU_MODE_ALWAYS, GRIS_TU_MODE_FAST and GRIS_TU_MODE_SLOW can be freely mixed. So can GRIS_TU_MODE_DISABLE_ALWAYS, GRIS_TU_MODE_DISABLE_FAST and GRIS_TU_MODE_DISABLE_SLOW. Defining GRIS_TU_MODE_* and the corresponding GRIS_TU_MODE_DISABLE_* will result in a compile-time error.
- GRIS_TU_MODE_NONE - convenience macro, none of the assert macros will be active.
- GRIS_TU_MODE_ALWAYS - GRIS_ALAWYS_ASSERT will be active.
- GRIS_TU_MODE_DISABLE_ALWAYS - GRIS_ALAWYS_ASSERT will not be active.
- GRIS_TU_MODE_FAST - GRIS_FAST_ASSERT will be active.
- GRIS_TU_MODE_DISABLE_FAST - GRIS_FAST_ASSERT will not be active.
- GRIS_TU_MODE_SLOW - GRIS_SLOW_ASSERT will be active.
- GRIS_TU_MODE_DISABLE_SLOW - GRIS_SLOW_ASSERT will not be active.

# Assertion failure callbacks
The library makes a distinction between the failure logging callback and the failure handler.

The logging callback has the following signature:

`void LoggingCallback(const char* file, uint32_t line, const char* format, va_list args)`

It is responsible for formatting the assertion failure message and sending it to the appropriate logging mechanism. The callback can be set by calling `Gris::Assert::SetLoggingCallback`.

Library provides the following callbacks:
- StdoutLoggigCallback - logs the formatted message to `STDOUT`.
- StderrLoggigCallback - logs the formatted message to `STDERR`.
- NullLoggigCallback - does not log anything.

The failure handler has the following signature:

`void FailureCallback()`

It is responsible for taking the appropriate action when the assert fails. The handler can be set by calling `Gris::Assert::SetFailureHandler`.

Library provides the following handlers:
- AbortHandler - calls `abort()`.
- ThrowHandler - throws `Gris::Assert::AssertionException`.
- NullHandler - does not take any actions.

# Checking if assertion macro is enabled
The library provides macros for conditional compilation. This is useful in the headers when introducing code that would break ABI compatibility, which is only allowed when GRIS_TARGET_MODE_SAFE is defined. The macros are:
- GRIS_ALWAYS_ASSERT_IS_ACTIVE
- GRIS_FAST_ASSERT_IS_ACTIVE
- GRIS_SLOW_ASSERT_IS_ACTIVE
