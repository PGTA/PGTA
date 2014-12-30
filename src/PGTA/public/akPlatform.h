
#pragma once

#ifdef _WIN32
#   ifdef PGTA_EXPORTS
#       define PGTA_API __declspec(dllexport)
#   else
#       define PGTA_API __declspec(dllimport)
#   endif
#else
#   ifdef PGTA_EXPORTS
#       define PGTA_API __attribute__ ((visibility("default")))
#   else
#       define PGTA_API
#   endif
#endif
