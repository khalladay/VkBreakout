#include <stdio.h>
#include "os_support.h"
#include "Renderer.h"
#include "Primitive.h"
#include "Mesh.h"

#define APP_NAME "Vulkan Breakout"
#define SCREEN_W 1280
#define SCREEN_H 720

Renderer* renderer;
Mesh* rectMesh;

Primitive paddlePrim;

void mainLoop();
void createMeshes();

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE pInstance, LPSTR cmdLine, int showCode)
{
	HWND wndHdl = makeWindow(Instance, APP_NAME, SCREEN_W, SCREEN_H);
	renderer = new Renderer(SCREEN_W, SCREEN_H, Instance, wndHdl, APP_NAME);

	createMeshes();

	paddlePrim.col = glm::vec4(1, 0, 0, 1);
	paddlePrim.scale = glm::vec3(1, 1, 1);
	paddlePrim.pos = glm::vec4(0.0, 0.5, 0.5, 0.5);
	paddlePrim.meshResource = rectMesh;

	mainLoop();

	return 0;
}

void createMeshes()
{
	const std::vector<Mesh::Vertex> rectVerts = {
		{{ 5.0f, 1.0f, 0.0f }},
		{{ 0.0f, 1.0f, 0.0f }},
		{{ 0.0f, 0.0f, 0.0f }},
		{{ 5.0f, 0.0f, 0.0f }},
	};

	const std::vector<unsigned short> rectIndices =
	{
		0,1,2,1,2,3
	};

	rectMesh = new Mesh(rectVerts, rectIndices, renderer);
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

		renderer->draw(nullptr, nullptr, &paddlePrim);

	}
}
