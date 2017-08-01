#include <stdio.h>
#include "os_support.h"
#include "Renderer.h"
#include "stdafx.h"
#include "BreakoutGame.h"

Renderer* renderer;
BreakoutGame* game;

void mainLoop();

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE pInstance, LPSTR cmdLine, int showCode)
{
	HWND wndHdl = makeWindow(Instance, APP_NAME, SCREEN_W, SCREEN_H);
	renderer = new Renderer(SCREEN_W, SCREEN_H, Instance, wndHdl, APP_NAME);
	game = new BreakoutGame(renderer);


	mainLoop();

	return 0;
}

void mainLoop()
{
	bool running = true;
	long long lastFrame = GetMilliseconds();
	double fpsAccum = 0.0;
	int count = 0;

	while (running)
	{
		long long thisFrameTime = GetMilliseconds();
		long long deltaTime = (thisFrameTime - lastFrame);
		lastFrame = thisFrameTime;
		fpsAccum += deltaTime * 10.0f;

		if (count++ == 49)
		{
			printf("Frametime (avg of past 50 frames): %f ms\n", fpsAccum / 50.0f);
			count = 0;
			fpsAccum = 0;
		}

		HandleOSEvents();

		if (GetKey(KEY_ESCAPE))
		{
			running = false;
		}

		if (game->isGameOver())
		{
			delete game;;
			game = new BreakoutGame(renderer);
		}

		game->tick(deltaTime/100.0f);
		game->draw();
	}
}
