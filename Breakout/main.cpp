#include <stdio.h>
#include "stdafx.h"

#include "os_support.h"
#include "Renderer.h"
#include "BreakoutGame.h"

Renderer* renderer;
void mainLoop();

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE pInstance, LPSTR cmdLine, int showCode)
{
	HWND wndHdl = OS::makeWindow(Instance, APP_NAME, SCREEN_W, SCREEN_H);

	renderer = new Renderer(Instance, wndHdl, APP_NAME);

	Breakout::newGame();

	mainLoop();

	return 0;
}

void mainLoop()
{
	bool running = true;

	long long lastFrame = OS::getMilliseconds();
	
	double fpsAccum = 0.0;
	int count = 0;

	while (running)
	{
		long long thisFrameTime = OS::getMilliseconds();
		long long deltaTime = (thisFrameTime - lastFrame);
		lastFrame = thisFrameTime;
		fpsAccum += deltaTime * 10.0f;

		if (count++ == 49)
		{
			printf("Frametime (avg of past 50 frames): %f ms\n", fpsAccum / 50.0f);
			count = 0;
			fpsAccum = 0;
		}

		OS::handleOSEvents();

		if (OS::getKey(OS::KEY_ESCAPE))
		{
			running = false;
		}

		if (Breakout::isGameOver())
		{
			Breakout::newGame();
		}

		Breakout::tick(deltaTime/100.0f);
		Breakout::draw(renderer);
	}
}
