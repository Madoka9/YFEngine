#pragma once
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)

#include "simple_library/public/simple_library.h"
#define EG_LOG(format, ...) log_log(format, ##__VA_ARGS__)
#define EG_LOG_ERROR(format, ...) log_error(format, ##__VA_ARGS__)
#define EG_LOG_WARNING(format, ...) log_warning(format, ##__VA_ARGS__)
#define EG_LOG_SUCCESS(format, ...) log_success(format, ##__VA_ARGS__)
#elif 0
#endif

