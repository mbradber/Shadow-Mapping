#pragma once

class Timer
{
public:
	Timer();
	
	void start();
	float getGameTime();
	bool updateFPS();
	double getDelta();

private:
	
	__int64 gameTime;
	__int64 initTime;

	bool fpsMutex;
	float currTime;
	float lastDelta;
	
};

