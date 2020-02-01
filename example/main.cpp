#define SEPD_DEBUG 1

#include <iostream>
#include <vector>

#include <chrono>
#include <sepd.h>

using namespace m2d;

enum event
{
	key_pressed
};

int main(int argc, char *argv[])
{
	auto detectors = {
		sepd::detector::create(
		    "double",
		    { sepd::event_t(key_pressed),
		        sepd::event_t(key_pressed, 200) },
		    [] {
		        std::cout << "detected2 : " << std::endl;
		    },
		    200),
		sepd::detector::create(
		    "single",
		    { sepd::event_t(key_pressed) },
		    [] {
		        std::cout << "detected1 : " << std::endl;
		    },
		    200),
		sepd::detector::create(
		    "triple",
		    { sepd::event_t(key_pressed),
		        sepd::event_t(key_pressed, 200),
		        sepd::event_t(key_pressed, 200) },
		    [] {
		        std::cout << "detected3 : " << std::endl;
		    },
		    200),
		sepd::detector::create(
		    "quad",
		    { sepd::event_t(key_pressed),
		        sepd::event_t(key_pressed, 200),
		        sepd::event_t(key_pressed, 200),
		        sepd::event_t(key_pressed, 200) },
		    [] {
		        std::cout << "detected 4: " << std::endl;
		    },
		    200)
	};

	sepd::decetor_group group(detectors);

	std::istream::char_type ch;
	do {
		group.touch(key_pressed);
	} while ((ch = std::cin.get()) != 'q');

	return 0;
}
