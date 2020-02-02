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
	private:
		int step_ = -1;

	public:
		void reset()
		{
			step_ = -1;
		}

		void go_next()
		{
			step_++;
		}

		int next_step()
		{
			return step_ + 1;
		}

		int previous_step()
		{
			return std::max(-1, step_ - 1);
		}

		int current_step()
		{
			return step_;
		}
	};

	class detector : public std::enable_shared_from_this<detector>
	{
	public:
		enum state
		{
			initial,
			detecting,
			checking,
			in_progress,
			invalid,
			activated
		};

		enum action
		{
			touch,
			accept,
			reject,
			eval,
			expire
		};

		std::string to_string(state s)
		{
			switch (s) {
				case state::initial:
					return "initial";
				case state::detecting:
					return "detectiong";
				case state::checking:
					return "checking";
				case state::in_progress:
					return "in_progress";
				case state::invalid:
					return "invalid";
				case state::activated:
					return "activated";
				default:
					return "unkwond";
			}
		}

		std::string to_string(action a)
		{
			switch (a) {
				case action::touch:
					return "touch";
				case action::accept:
					return "accept";
				case action::reject:
					return "reject";
				case action::eval:
					return "eval";
				case action::expire:
					return "expire";
				default:
					return "unkwond";
			}
		}

	private:
		sequence seq_;
		state current_state_ = state::initial;
		unsigned int delay_msec_ = 0;
		std::chrono::system_clock::time_point last_event_time_;
		std::vector<event_t> event_table_;
		std::function<void()> handler_;
		std::string name_ = "";

		detector(std::string name, std::vector<event_t> event_table, std::function<void()> handler, unsigned int delay_msec = 0)
		    : delay_msec_(delay_msec)
		    , event_table_(event_table)
		    , handler_(handler)
		    , name_(name)
		{
			change_state(state::initial);
		}

		void dispatch(int delay_msec)
		{
			if (delay_msec == 0) {
				handler_();
			}
			else {
				std::weak_ptr<detector> weak_this = this->shared_from_this();
				auto duration = std::chrono::milliseconds { delay_msec };
				std::thread t([=] {
					std::this_thread::sleep_for(duration);
					auto shared_this = weak_this.lock();
					if (shared_this && shared_this->current_state_ == detector::state::activated) {
						auto current_time = std::chrono::system_clock::now();
						auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - shared_this->last_event_time_).count();
						util::print_log(&shared_this, shared_this->name_ + "=> handler elapsed: delay " + std::to_string(elapsed) + "[ms]");
						shared_this->handler_();
					}
				});
				t.detach();
			}
		}

		void evalate_event(int event, int elapsed)
		{
			auto timing = event_table_[next_step()];
			std::weak_ptr<detector> weak_this = this->shared_from_this();
			timing.acceptable(event, elapsed, [=](bool result) {
				if (auto shared_this = weak_this.lock()) {
					if (shared_this->current_state_ != detector::state::detecting) {
						return;
					}
					util::print_log(&shared_this, shared_this->name_ + "=> result");
					if (result == false) {
						// failed
						util::print_log(&shared_this, shared_this->name_ + "=> failed");
						shared_this->update_state(action::reject, event, elapsed);
					}
					else {
						// go ahead
						util::print_log(&shared_this, shared_this->name_ + "=> go ahead");
						shared_this->update_state(action::accept, event, elapsed);
					}
				}
			});
		}

		detector::state detect_state(int step, int completion_step)
		{
			if (step == completion_step) {
				// util::print_log(this, "activated");
				return state::activated;
			}
			else if (step > completion_step) {
				// util::print_log(this, "invalid");
				return state::invalid;
			}
			else {
				// util::print_log(this, "in_progress");
				return state::in_progress;
			}
		}

		void update_state(detector::action action, int event, int elapsed)
		{
			util::print_log(this, name_ + "=> update state: current = " + detector::to_string(current_state_) + ", action = " + detector::to_string(action) + ", elapsed = " + std::to_string(elapsed) + "[ms]");
			switch (current_state_) {
				case detector::state::initial:
					if (action == action::touch) {
						change_state(state::detecting);
						evalate_event(event, elapsed);
					}
					break;
				case detector::state::detecting:
					if (action == action::touch) {
						change_state(state::invalid);
					}
					else if (action == action::accept) {
						go_next_sequence();
						change_state(state::checking);
						update_state(action, event, elapsed);
					}
					else if (action == action::reject) {
						change_state(state::invalid);
					}
					break;
				case detector::state::checking:
					change_state(detect_state(seq_.current_step(), event_table_.size() - 1));
					update_state(action::eval, event, elapsed);
					break;
				case detector::state::in_progress:
					if (action == action::touch) {
						change_state(state::detecting);
						evalate_event(event, elapsed);
					}
					else if (action == action::expire) {
						expired(event, elapsed);
					}
					break;
				case detector::state::activated:
					if (action == action::touch) {
						change_state(state::invalid);
					}
					else if (action == action::expire) {
						expired(event, elapsed);
					}
					else if (action == action::eval) {
						dispatch(delay_msec_);
					}
					break;
				case detector::state::invalid:
					if (action == action::expire) {
						expired(event, elapsed);
					}
					break;
				default:
					break;
			}
		}

		void expired(int event, int elapsed)
		{
			change_state(state::initial);
			update_state(action::touch, event, elapsed);
		}

		void go_next_sequence()
		{
			seq_.go_next();
		}

		void reset_sequence()
		{
			seq_.reset();
		}

	public:
		static std::shared_ptr<detector> create(std::string name, std::vector<event_t> event_table, std::function<void()> handler, unsigned int delay_msec = 0)
		{
			auto d = new detector(name, event_table, handler, delay_msec);
			return std::shared_ptr<detector>(std::move(d));
		}

		detector::state change_state(detector::state state)
		{
			util::print_log(this, name_ + "=> change state " + detector::to_string(state));
			if (state == detector::state::initial) {
				reset_sequence();
			}

			current_state_ = state;
			return current_state_;
		}

		void input(int event)
		{
			auto current_time = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_event_time_).count();
			switch (current_state_) {
				case detector::state::initial:
					update_state(action::touch, event, elapsed);
					break;
				case detector::state::detecting:
					// when receive event during detecting state,
					// change state to invalid
					change_state(detector::state::invalid);
					break;
				case detector::state::in_progress:
					if (delay_msec_ < elapsed) {
						// expire
						update_state(action::expire, event, elapsed);
					}
					else {
						update_state(action::touch, event, elapsed);
					}
					break;
				case detector::state::activated:
					if (delay_msec_ < elapsed) {
						// expire
						update_state(action::expire, event, elapsed);
					}
					else {
						update_state(action::touch, event, elapsed);
					}
					break;
				case detector::state::invalid:
					if (delay_msec_ < elapsed) {
						// expire
						update_state(action::expire, event, elapsed);
					}
					break;
				default:
					break;
			}
			last_event_time_ = std::chrono::system_clock::now();
		}

		int current_step()
		{
			return seq_.current_step();
		}

		int next_step()
		{
			return seq_.next_step();
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

		void input(int event)
		{
			for (auto d : detectors_) {
				d->input(event);
			}
		}
	};
}
}
