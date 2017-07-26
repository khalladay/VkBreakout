#include <stdio.h>
#include "os_support.h"
#include "Renderer.h"
#include "Primitive.h"
#include "Mesh.h"
#include "stdafx.h"
#include "BreakoutGame.h"

Renderer* renderer;
Mesh* rectMesh;

Primitive paddlePrim;

BreakoutGame* game;

void mainLoop();
void createMeshes();

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

	while (running)
	{
		HandleOSEvents();

		if (GetKey(KEY_ESCAPE))
		{
			running = false;
		}

		game->tick(1.0f);
		game->draw();
	}
}
