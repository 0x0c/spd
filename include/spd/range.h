#pragma once

namespace m2d
{
namespace spd
{
	class range_t
	{
		unsigned int start_msec_ = 0;
		unsigned int end_msec_ = 0;

	public:
		unsigned int start_msec()
		{
			return start_msec_;
		}
		unsigned int end_msec()
		{
			return end_msec_;
		}

		range_t(unsigned int start_msec, unsigned int end_msec)
		    : start_msec_(start_msec)
		    , end_msec_(end_msec)
		{
		}

		range_t(unsigned int value)
		    : end_msec_(value)
		{
		}

		range_t()
		    : start_msec_(0)
		    , end_msec_(0)
		{
		}

		bool contains(unsigned int value)
		{
			if (start_msec() == 0 && end_msec() == 0) {
				return true;
			}
			else if (start_msec() <= value && value <= end_msec()) {
				return true;
			}
			return false;
		}
	};
}
}