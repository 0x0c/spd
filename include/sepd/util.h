#pragma once

#include "detector.h"

namespace m2d
{
namespace sepd
{
	class util
	{
	public:
		static void print_log(void *instance, std::string str)
		{
#ifdef SEPD_DEBUG
			printf("[%p] %s\n", instance, str.c_str());
#endif
		}
	};
}
}