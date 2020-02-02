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
	class sequence
	{
	public:
		enum state
		{
			initial,
			in_progress,
			invalid,
			activated
		};

	private:
		int step_ = -1;
		int completion_step_ = -1;

	public:
		static std::string to_string(sequence::state s)
		{
			switch (s) {
				case sequence::initial:
					return "initial";
				case sequence::state::in_progress:
					return "in_progress";
				case sequence::state::invalid:
					return "invalid";
				case sequence::state::activated:
					return "activated";
				default:
					return "undefined";
			}
		}

		sequence(int completion_step)
		    : completion_step_(completion_step)
		{
			reset();
		}

		int next_step()
		{
			return step_ + 1;
		}

		int previous_step()
		{
			return std::max(-1, step_ - 1);
		}

		void go_next()
		{
			step_++;
		}

		void reset()
		{
			step_ = -1;
		}

		int current_step()
		{
			return step_;
		}

		state current_state()
		{
			if (step_ == completion_step_) {
				// util::print_log(this, "activated");
				return state::activated;
			}
			else if (step_ > completion_step_) {
				// util::print_log(this, "invalid");
				return state::invalid;
			}
			else if (step_ == -1) {
				return state::initial;
			}

			// util::print_log(this, "in_progress");
			return state::in_progress;
		}
	};

	class detector : public std::enable_shared_from_this<detector>
	{
	private:
		sequence seq_;
		unsigned int delay_msec_ = 0;
		std::chrono::system_clock::time_point last_event_time_;
		std::vector<event_t> event_table_;
		std::function<void()> handler_;
		std::string name_ = "";

		void go_next_sequence()
		{
			seq_.go_next();
		}

		int current_sequence()
		{
			return seq_.current_state();
		}

		detector(std::string name, std::vector<event_t> event_table, std::function<void()> handler, unsigned int delay_msec = 0)
		    : seq_(sequence(event_table.size() - 1))
		    , delay_msec_(delay_msec)
		    , event_table_(event_table)
		    , handler_(handler)
		    , name_(name)
		{
			reset_sequence();
		}

		void dispatch(sequence::state state, unsigned int delay_msec)
		{
			if (state == sequence::state::activated) {
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
			return current_state() == sequence::state::in_progress || current_state() == sequence::state::initial;
		}

	public:
		static std::shared_ptr<detector> create(std::string name, std::vector<event_t> event_table, std::function<void()> handler, unsigned int delay_msec = 0)
		{
			auto d = new detector(name, event_table, handler, delay_msec);
			return std::shared_ptr<detector>(std::move(d));
		}

		void reset_sequence()
		{
			seq_.reset();
		}

		int next_step()
		{
			return seq_.next_step();
		}

		void touch(int event)
		{
			auto current_time = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_event_time_).count();

			util::print_log(this, this->name_ + "=> current state: " + sequence::to_string(current_state()) + ", " + std::to_string(elapsed) + "[ms]");
			util::print_log(this, this->name_ + "=> current sequence: " + std::to_string(current_sequence()));

			if (is_valid() == false) {
				util::print_log(this, this->name_ + "=> invalid");
				if (delay_msec_ < elapsed) {
					// down time finished
					util::print_log(this, this->name_ + "=> down time finished, reset sequence");
					reset_sequence();
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
			}

			if (is_valid()) {
				// check
				util::print_log(this, this->name_ + "=> check: next seq " + std::to_string(next_step()));
				auto timing = event_table_[next_step()];

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

						util::print_log(&shared_this, shared_this->name_ + "=> current state: " + sequence::to_string(shared_this->current_state()));
						util::print_log(&shared_this, shared_this->name_ + "=> current sequence: " + std::to_string(shared_this->current_sequence()));
					}
				});
			}
			last_event_time_ = std::chrono::system_clock::now();
		}

		sequence::state current_state()
		{
			return seq_.current_state();
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