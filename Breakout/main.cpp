#include <stdio.h>
#include "stdafx.h"

#include "os_support.h"
#include "Renderer.h"
#include "BreakoutGame.h"

void mainLoop();

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE pInstance, LPSTR cmdLine, int showCode)
{
	HWND wndHdl = OS::makeWindow(Instance, APP_NAME, SCREEN_W, SCREEN_H);
	Renderer::initializeRendering(Instance, wndHdl, APP_NAME);

	Breakout::newGame();

	mainLoop();

	return 0;
}

void mainLoop()
{
	bool running = true;

	double lastFrame = OS::getMilliseconds();
	
	double fpsAccum = 0.0;
	int count = 0;

	while (running)
	{
		double thisFrameTime = OS::getMilliseconds();
		double deltaTime = (thisFrameTime - lastFrame);
		lastFrame = thisFrameTime;
		fpsAccum += deltaTime;

		if (count++ == 999)
		{
			printf("Frametime (avg of past 1000 frames): %f ms\n", fpsAccum / 1000.0f);
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

		Breakout::tick(0.01f);
		Breakout::draw();
	}
}
