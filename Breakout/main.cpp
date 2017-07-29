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
	double lastFrame = GetTime();

	while (running)
	{
		double thisFrameTime = GetTime();
		double deltaTime = thisFrameTime - lastFrame;
		lastFrame = thisFrameTime;

		HandleOSEvents();

		if (GetKey(KEY_ESCAPE))
		{
			running = false;
		}

		game->tick(deltaTime);
		game->draw();
	}
}
