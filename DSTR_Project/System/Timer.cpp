#include <Pch/Pch.h>
#include "Timer.h"

Timer::Timer()
{
}

void Timer::start()
{
	startPoint = std::chrono::steady_clock::now();
}

void Timer::stop()
{
	endPoint = std::chrono::steady_clock::now();
}

void Timer::print()
{
	std::cout << "Time : " << std::chrono::duration_cast<std::chrono::microseconds>(endPoint - startPoint).count() << " MicroSec" <<std::endl;
}
