#pragma once

#include <functional>
#include <thread>

#include "range.h"
#include "util.h"

namespace m2d
{
namespace spd
{
	class pattern
	{
	public:
		enum behaviour
		{
			discrete,
			continuous
		};

	private:
		int pattern_;
		range interval_;
		behaviour accteptable_behaviour_ = discrete;

	public:
		pattern(int pattern, range interval, pattern::behaviour accteptable_behaviour = behaviour::discrete)
		    : pattern_(pattern)
		    , interval_(interval)
		    , accteptable_behaviour_(accteptable_behaviour)
		{
		}

		pattern(int pattern)
		    : pattern_(pattern)
		    , accteptable_behaviour_(behaviour::discrete)
		{
		}

		unsigned int start_msec()
		{

			return interval_.start_msec();
		}

		unsigned int end_msec()
		{

			return interval_.end_msec();
		}
		bool acceptable_pattern(int pattern)
		{
			return pattern_ == pattern;
		}

		bool acceptable(int pattern, unsigned int duration)
		{
			util::print_log(this, "acceptable: contain = " + std::to_string(interval_.contains(duration)) + ", type = " + std::to_string(acceptable_pattern(pattern)));
			return interval_.contains(duration) && acceptable_pattern(pattern);
		}

		behaviour pattern_behaviour()
		{
			return accteptable_behaviour_;
		}
	};
}
}
