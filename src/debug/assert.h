#ifndef __debug_assert_h__
#define __debug_assert_h__

#include <assert.h>
#include <stdio.h>

#ifndef NDEBUG

#if defined(__linux__)
	#include <signal.h>
	#define HE_DEBUGBREAK() raise(SIGTRAP)
#elif defined(_WIN32)
	#define HE_DEBUGBREAK() __debugbreak()
#else
	#error "This platform has no known debugbreak"
	#define HE_DEBUGBREAK()
#endif

#else /* NDEBUG */
#define HE_DEBUGBREAK()
#endif

#ifndef NDEBUG
#define HE_ASSERT(CONDITION, MSG) if (!(CONDITION)) { fprintf(stderr, "error in %s in function %s:%i: assertion (%s) failed: %s\n", __FILE__, __func__, __LINE__, #CONDITION, MSG); HE_DEBUGBREAK(); }
#define HE_VERIFY(CONDITION, MSG) HE_ASSERT(CONDITION, MSG)
#else
#define HE_ASSERT(CONDITION, MSG)
#define HE_VERIFY(CONDITION, MSG) (CONDITION)
#endif

#endif /* __debug_assert_h__ */
