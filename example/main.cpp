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
		    "1",
		    { sepd::event_t(key_pressed) },
		    [] {
		        std::cout << "detected1 : " << std::endl;
		    },
		    200),
		 sepd::detector::create(
		     "2",
		     { sepd::event_t(key_pressed),
		         sepd::event_t(key_pressed, 200) },
		     [] {
		         std::cout << "detected2 : " << std::endl;
		     },
		     200),
		 sepd::detector::create(
		     "3",
		     { sepd::event_t(key_pressed),
		         sepd::event_t(key_pressed, 200),
		         sepd::event_t(key_pressed, 200) },
		     [] {
		         std::cout << "detected3 : " << std::endl;
		     },
		     200),
		 sepd::detector::create(
		     "4",
		     { sepd::event_t(key_pressed),
		         sepd::event_t(key_pressed, 200),
		         sepd::event_t(key_pressed, 200),
		         sepd::event_t(key_pressed, 200) },
		     [] {
		         std::cout << "detected4: " << std::endl;
		     },
		     200)
	};

	sepd::decetor_group group(detectors);

    
    std::cout << "start" << std::endl;
	std::istream::char_type ch;
	while ((ch = std::cin.get()) != 'q') {
		group.touch(key_pressed);
	}

	return 0;
}
