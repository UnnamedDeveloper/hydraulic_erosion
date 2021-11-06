#ifndef __debug_assert_h__
#define __debug_assert_h__

#include <assert.h>

#ifndef NDEBUG
#define HE_ASSERT(CONDITION) assert(CONDITION)
#define HE_VERIFY(CONDITION) HE_ASSERT(CONDITION)
#else
#define HE_ASSERT(CONDITION)
#define HE_VERIFY(CONDITION) (CONDITION)
#endif

#endif /* __debug_assert_h__ */
