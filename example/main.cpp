#define SPD_DEBUG 1

#include <iostream>
#include <vector>

#include <chrono>
#include <spd.h>

using namespace m2d;

enum event
{
	key_release,
	key_pressed
};

int main(int argc, char *argv[])
{
	auto detectors = {
		spd::detector::create(
		    "long",
		    { spd::pattern_t(key_pressed, 500, spd::pattern_t::behaviour::continuous) },
		    [] {
		        std::cout << ">>>>>>>>>>>> detect long" << std::endl;
		    },
		    200),
		spd::detector::create(
		    "shor-long-short",
		    {
		        // short
		        spd::pattern_t(key_pressed),
		        spd::pattern_t(key_release, 200),
		        //long
		        spd::pattern_t(key_pressed, 500, spd::pattern_t::behaviour::continuous),
		        spd::pattern_t(key_release, 200),
		        //short
		        spd::pattern_t(key_pressed, 200),
		        spd::pattern_t(key_release, 200),
		    },
		    [] {
		        std::cout << ">>>>>>>>>>>> detect short-long-short" << std::endl;
		    },
		    200),
		spd::detector::create(
		    "1",
		    { spd::pattern_t(key_pressed),
		        spd::pattern_t(key_release, 200) },
		    [] {
		        std::cout << ">>>>>>>>>>>> detected 1" << std::endl;
		    },
		    200),
		spd::detector::create(
		    "2",
		    { spd::pattern_t(key_pressed),
		        spd::pattern_t(key_release, 200),
		        spd::pattern_t(key_pressed, 200),
		        spd::pattern_t(key_release, 200) },
		    [] {
		        std::cout << ">>>>>>>>>>>> detected 2" << std::endl;
		    },
		    200),
		spd::detector::create(
		    "3",
		    { spd::pattern_t(key_pressed),
		        spd::pattern_t(key_release, 200),
		        spd::pattern_t(key_pressed, 200),
		        spd::pattern_t(key_release, 200),
		        spd::pattern_t(key_pressed, 200),
		        spd::pattern_t(key_release, 200) },
		    [] {
		        std::cout << ">>>>>>>>>>>> detected 3" << std::endl;
		    },
		    200),
		spd::detector::create(
		    "4",
		    { spd::pattern_t(key_pressed),
		        spd::pattern_t(key_release, 200),
		        spd::pattern_t(key_pressed, 200),
		        spd::pattern_t(key_release, 200),
		        spd::pattern_t(key_pressed, 200),
		        spd::pattern_t(key_release, 200),
		        spd::pattern_t(key_pressed, 200),
		        spd::pattern_t(key_release, 200) },
		    [] {
		        std::cout << ">>>>>>>>>>>> detected 4" << std::endl;
		    },
		    200)
	};

	spd::decetor_group group(detectors);

	std::cout << "start" << std::endl;
	std::istream::char_type ch;
	while ((ch = std::cin.get()) != 'q') {
		if (ch == 'z') {
			// long
			group.input(key_pressed);
		}
		else if (ch == 'x') {
			// short-long-short
			// short
			group.input(key_pressed);
			std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
			group.input(key_release);

			// long
			group.input(key_pressed);
			std::this_thread::sleep_for(std::chrono::milliseconds { 510 });
			group.input(key_release);

			// short
			group.input(key_pressed);
			std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
			group.input(key_release);
		}
		else if (ch == '1') {
			// 1
			group.input(key_pressed);
			std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
			group.input(key_release);
		}
		else if (ch == '2') {
			// 2
			for (int i = 0; i < 2; i++) {
				group.input(key_pressed);
				std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
				group.input(key_release);
				std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
			}
		}
		else if (ch == '3') {
			// 3
			for (int i = 0; i < 3; i++) {
				group.input(key_pressed);
				std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
				group.input(key_release);
				std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
			}
		}
		else if (ch == '4') {
			// 4
			for (int i = 0; i < 4; i++) {
				group.input(key_pressed);
				std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
				group.input(key_release);
				std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
			}
		}
		else if (ch == '5') {
			// invalid pattern
			group.input(key_pressed);
			std::this_thread::sleep_for(std::chrono::milliseconds { 600 });
			group.input(key_release);
		}
	}

	return 0;
}
