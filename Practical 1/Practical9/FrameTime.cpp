#include "Frametime.h"

void Frametime::init(int fps)
{
	QueryPerformanceFrequency(&timerFreq);
	QueryPerformanceCounter(&timeNow);
	QueryPerformanceCounter(&timePrevious);

	requestedFPS = fps;

	intervalsPerFrame = timerFreq.QuadPart / requestedFPS;
}

int Frametime::FramesToUpdate()
{
	int framesToUpdate = 0;
	QueryPerformanceCounter(&timeNow);

	intervalsSinceLastUpdate = timeNow.QuadPart - timePrevious.QuadPart;

	framesToUpdate = (int)(intervalsSinceLastUpdate / intervalsPerFrame);

	if (framesToUpdate != 0) {
		QueryPerformanceCounter(&timePrevious);
	}
	return framesToUpdate;
}
