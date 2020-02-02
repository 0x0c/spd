#pragma once

#include <functional>
#include <thread>

#include "range.h"
#include "util.h"

namespace m2d
{
namespace sepd
{
	class event_t
	{
	public:
		enum behaviour
		{
			discrete,
			continuous
		};

	private:
		int event_type_;
		range_t interval_;
		behaviour accteptable_behaviour_ = discrete;

	public:
		event_t(int event_type, range_t interval, event_t::behaviour accteptable_behaviour = behaviour::discrete)
		    : event_type_(event_type)
		    , interval_(interval)
		    , accteptable_behaviour_(accteptable_behaviour)
		{
		}

		event_t(int event_type)
		    : event_type_(event_type)
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
		bool acceptable_event(int event_type)
		{
			return event_type_ == event_type;
		}

		bool acceptable(int event_type, unsigned int duration)
		{
			util::print_log(this, "acceptable: contain = " + std::to_string(interval_.contains(duration)) + ", type = " + std::to_string(acceptable_event(event_type)));
			return interval_.contains(duration) && acceptable_event(event_type);
		}

		behaviour event_behaviour()
		{
			return accteptable_behaviour_;
		}
	};
}
}
