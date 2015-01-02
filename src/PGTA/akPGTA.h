
#ifndef AK_PGTA
#define AK_PGTA

#if !defined(AK_USE_PGTA_C) && !defined(__cplusplus)
#   define AK_USE_PGTA_C
#endif

#ifndef AK_USE_PGTA_C
#include "public/akPGTAcpp.h"
#else
#include "public/akPGTAc.h"
#endif

#endif // AK_PGTA
