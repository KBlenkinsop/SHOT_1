#pragma once

#include "cuckoo/time/time.h"
#include <fstream>

class timer
{
public:
	timer()
	{
		clock_frequency = cuckoo::get_cpu_frequency();
	}

	void start_timer()
	{
		time_stamp_start = cuckoo::get_cpu_time();
	}
	void end_timer()
	{
		time_stamp_end = cuckoo::get_cpu_time();
	}
	float get_elapsed_time_secs() const
	{
		double elapsed_secs = (double)(time_stamp_end - time_stamp_start) / (double)clock_frequency;
		return elapsed_secs;
	}

	u64 clock_frequency = {};
	u64 time_stamp_start = {}, time_stamp_end = {};
};