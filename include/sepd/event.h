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

		bool acceptable(int event_type, unsigned int duration)
		{
			util::print_log(this, "acceptable: contain = " + std::to_string(interval_.contains(duration)) + ", type = " + std::to_string(event_type_ == event_type));
			return interval_.contains(duration) && event_type_ == event_type;
		}

	public:
		void acceptable(int event_type, unsigned int duration, std::function<void(bool)> result)
		{
			if (accteptable_behaviour_ == behaviour::discrete) {
				result(acceptable(event_type, duration));
			}
			else {
				// std::weak_ptr<event_t> weak_this = this->shared_from_this();
				// std::thread t([=] {
				// 	std::this_thread::sleep_for(std::chrono::milliseconds { shared_this->interval_.start_msec() });
				// 	auto shared_this = weak_this.lock();
				// 	if (!shared_this) {
				// 		return;
				// 	}
				// });
				// t.detach();
			}
		}

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
	};
}
}
