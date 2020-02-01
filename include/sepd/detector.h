#pragma once

#include <chrono>
#include <functional>
#include <thread>

#include "event.h"
#include "util.h"

namespace m2d
{
namespace sepd
{
	class detector : public std::enable_shared_from_this<detector>
	{
	public:
		enum state
		{
			in_progress,
			invalid,
			activated
		};

		static std::string to_string(state s)
		{
			switch (s) {
				case detector::state::in_progress:
					return "in_progress";
				case detector::state::invalid:
					return "invalid";
				case detector::state::activated:
					return "activated";
				default:
					return "undefined";
			}
		}

	private:
		int sequence_ = -1;
		unsigned int delay_msec_ = 0;
		std::chrono::system_clock::time_point last_event_time_;
		std::vector<event_t> event_table_;
		std::function<void()> handler_;
		std::string name_ = "";

		void go_next_sequence()
		{
			sequence_++;
		}

		int current_sequence()
		{
			return sequence_;
		}

		detector(std::string name, std::vector<event_t> event_table, std::function<void()> handler, unsigned int delay_msec = 0)
		    : delay_msec_(delay_msec)
		    , event_table_(event_table)
		    , handler_(handler)
		    , name_(name)
		{
			reset_sequence();
		}

		void dispatch(state state, unsigned int delay_msec)
		{
			if (state == state::activated) {
				if (delay_msec == 0) {
					handler_();
				}
				else {
					std::weak_ptr<detector> weak_this = this->shared_from_this();
					std::thread t([=] {
						std::this_thread::sleep_for(std::chrono::milliseconds { delay_msec });
						auto shared_this = weak_this.lock();
						if (shared_this && state == shared_this->current_state()) {
							auto current_time = std::chrono::system_clock::now();
							auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - shared_this->last_event_time_).count();
							util::print_log(&shared_this, shared_this->name_ + "=> handler elapsed: delay " + std::to_string(elapsed) + "[ms]");
							shared_this->handler_();
						}
					});
					t.detach();
				}
			}
		}

		bool is_valid()
		{
			return current_state() == state::in_progress;
		}

	public:
		static std::shared_ptr<detector> create(std::string name, std::vector<event_t> event_table, std::function<void()> handler, unsigned int delay_msec = 0)
		{
			auto d = new detector(name, event_table, handler, delay_msec);
			return std::shared_ptr<detector>(std::move(d));
		}

		void reset_sequence()
		{
			sequence_ = -1;
		}

		void touch(int event)
		{
			auto current_time = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_event_time_).count();
			int next_sequence = current_sequence() + 1;

			util::print_log(this, this->name_ + "=> state: " + detector::to_string(current_state()) + ", " + std::to_string(elapsed) + "[ms]");
			util::print_log(this, this->name_ + "=> sequence: " + std::to_string(current_sequence()));

			if (is_valid() == false) {
				util::print_log(this, this->name_ + "=> invalid");
				if (delay_msec_ < elapsed) {
					// down time finished
					util::print_log(this, this->name_ + "=> down time finished, reset sequence");
					reset_sequence();
					next_sequence = current_sequence() + 1;
				}
				else {
					// still down time
					// invalid state
					util::print_log(this, this->name_ + "=> still down time");
					go_next_sequence();
				}
			}
			else if (delay_msec_ < elapsed) {
				// down time finished
				util::print_log(this, this->name_ + "=> expired, reset sequence");
				reset_sequence();
				next_sequence = current_sequence() + 1;
			}

			if (is_valid()) {
				// check
				util::print_log(this, this->name_ + "=> check: next seq " + std::to_string(next_sequence));
				auto timing = event_table_[next_sequence];

				std::weak_ptr<detector> weak_this = this->shared_from_this();
				timing.acceptable(event, elapsed, [=](bool result) {
					if (auto shared_this = weak_this.lock()) {
						util::print_log(&shared_this, shared_this->name_ + "=> result");
						if (result == false) {
							// failed
							util::print_log(&shared_this, shared_this->name_ + "=> failed");
							shared_this->reset_sequence();
						}
						else {
							// go ahead
							util::print_log(&shared_this, shared_this->name_ + "=> go ahead");
							shared_this->go_next_sequence();
						}
						auto s = shared_this->current_state();
						auto delay = shared_this->delay_msec_;
						shared_this->dispatch(s, delay);

						util::print_log(&shared_this, shared_this->name_ + "=> current state: " + detector::to_string(shared_this->current_state()));
						util::print_log(&shared_this, shared_this->name_ + "=> current sequence: " + std::to_string(shared_this->current_sequence()));
					}
				});
			}
			last_event_time_ = std::chrono::system_clock::now();
		}

		state current_state()
		{
			int table_size = event_table_.size();
			if (sequence_ == table_size - 1) {
				// util::print_log(this, "activated");
				return state::activated;
			}
			else if (sequence_ > table_size - 1) {
				// util::print_log(this, "invalid");
				return state::invalid;
			}

			// util::print_log(this, "in_progress");
			return state::in_progress;
		}
	};

	class decetor_group
	{
	private:
		std::vector<std::shared_ptr<detector>> detectors_;

	public:
		decetor_group(std::vector<std::shared_ptr<detector>> detectors)
		    : detectors_(detectors)
		{
		}

		void touch(int event)
		{
			for (auto d : detectors_) {
				d->touch(event);
			}
		}
	};
}
}