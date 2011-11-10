#include"Timer.h"
#include<Windows.h>

Timer::Timer()
:gameTime(0), initTime(0),
	fpsMutex(false), currTime(0), lastDelta(0)
{

}

void Timer::start()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&initTime);
}

float Timer::getGameTime()
{
	__int64 tempTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&tempTime);

	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);

	return (float)tempTime/countsPerSecond - (float)initTime/countsPerSecond;
}

bool Timer::updateFPS()
{
	if(!fpsMutex)
	{
		currTime = getGameTime();
		fpsMutex = true;
	}
	
	if(getGameTime() > currTime + 1)
	{
		fpsMutex = false;
		return true;
	}
	else
		return false;

}

double Timer::getDelta()
{
	float tDelta = getGameTime() - lastDelta;
	lastDelta = tDelta;
	return tDelta;
}