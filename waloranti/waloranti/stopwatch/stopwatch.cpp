#include "stopwatch.hpp"

stopwatch::stopwatch()
{
	this->update();
}

void stopwatch::update()
{
	QueryPerformanceCounter(&start_time);
}

double stopwatch::get_elapsed()
{
	LARGE_INTEGER current_time{ 0 }, elapsed_qpc{ 0 }, frequency{ 0 };
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&current_time);
	elapsed_qpc.QuadPart = current_time.QuadPart - start_time.QuadPart;

	double elapsedTime = ((static_cast<double>(elapsed_qpc.QuadPart) * 1000.0) /
		(static_cast<double>(frequency.QuadPart)));

	return elapsedTime;
}
