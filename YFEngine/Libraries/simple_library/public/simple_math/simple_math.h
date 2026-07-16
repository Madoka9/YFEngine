// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "simple_library/public/simple_library_macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef RAND_MAX
#define RAND_MAX 0x7fff
#endif

static float get_float_random_static()
{
	//重置时间戳
	//srand((unsigned int)time(nullptr));

	return (float)(rand()) / (float)RAND_MAX;
}

static float get_float_random_in_range(float in_min, float in_max)
{
	return in_max + get_float_random_static() * (in_max - in_min);
}

#ifdef __cplusplus
extern "C" {
#endif

unsigned int /*SIMPLE_LIBRARY_API*/ get_uint32_random(unsigned int in_max);

#ifdef __cplusplus
}
#endif
