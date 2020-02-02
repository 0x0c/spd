#define SEPD_DEBUG 1

#include <iostream>
#include <vector>

#include <chrono>
#include <sepd.h>

using namespace m2d;

enum event
{
	key_pressed,
	key_release
};

int main(int argc, char *argv[])
{
	auto detectors = {
		sepd::detector::create(
		    "1",
		    { sepd::event_t(key_pressed),
		        sepd::event_t(key_release, 200) },
		    [] {
		        std::cout << "detected1 : " << std::endl;
		    },
		    200),
		sepd::detector::create(
		    "2",
		    { sepd::event_t(key_pressed),
		        sepd::event_t(key_release, 200),
		        sepd::event_t(key_pressed, 200),
		        sepd::event_t(key_release, 200) },
		    [] {
		        std::cout << "detected2 : " << std::endl;
		    },
		    200),
		sepd::detector::create(
		    "3",
		    { sepd::event_t(key_pressed),
		        sepd::event_t(key_release, 200),
		        sepd::event_t(key_pressed, 200),
		        sepd::event_t(key_release, 200),
		        sepd::event_t(key_pressed, 200),
		        sepd::event_t(key_release, 200) },
		    [] {
		        std::cout << "detected3 : " << std::endl;
		    },
		    200),
		sepd::detector::create(
		    "4",
		    { sepd::event_t(key_pressed),
		        sepd::event_t(key_release, 200),
		        sepd::event_t(key_pressed, 200),
		        sepd::event_t(key_release, 200),
		        sepd::event_t(key_pressed, 200),
		        sepd::event_t(key_release, 200),
		        sepd::event_t(key_pressed, 200),
		        sepd::event_t(key_release, 200) },
		    [] {
		        std::cout << "detected4: " << std::endl;
		    },
		    200)
	};

	sepd::decetor_group group(detectors);

	std::cout << "start" << std::endl;
	std::istream::char_type ch;
	while ((ch = std::cin.get()) != 'q') {
		if (ch == 'z') {
            std::cout << "press" << std::endl;
			group.input(key_pressed);
		}
		else if (ch == 'x') {
            std::cout << "release" << std::endl;
			group.input(key_release);
		}
        else if (ch == 'c') {
            std::cout << "long press" << std::endl;
            group.input(key_pressed);
            std::this_thread::sleep_for(std::chrono::milliseconds { 500 });
            group.input(key_release);
        }
		else if (ch == '\n') {
            std::cout << "press-release" << std::endl;
			group.input(key_pressed);
			std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
			group.input(key_release);
		}
	}

	return 0;
}
