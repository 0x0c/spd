#pragma once

#include "detector.h"

namespace m2d
{
namespace spd
{
	class util
	{
	public:
		static void print_log(void *instance, std::string str)
		{
#ifdef SPD_DEBUG
			std::cout << std::this_thread::get_id() << "\t [" << instance << "] \t" << str << std::endl;
#endif
		}
	};
}
}