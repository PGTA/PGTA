
#ifndef AK_PGTA
#define AK_PGTA

#if !defined(AK_USE_PGTA_C) && !defined(__cplusplus)
#   define AK_USE_PGTA_C
#endif

#ifndef AK_USE_PGTA_C
#include "PGTA/akPGTAcpp.h"
#else
#include "PGTA/akPGTAc.h"
#endif

#endif // AK_PGTA
