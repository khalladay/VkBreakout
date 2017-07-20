#include <stdio.h>
#include "os_support.h"
#include "Renderer.h"
#include "Primitive.h"
#include "Mesh.h"
#include "stdafx.h"


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

	paddlePrim.col = glm::vec4(1, 0, 1, 1);
	paddlePrim.scale = glm::vec3(100.f, 100.0f, 1.0f);
	paddlePrim.pos = glm::vec4(-1000.0, 500.0, 0.0, 0.0);
	paddlePrim.meshResource = rectMesh;

	mainLoop();

	return 0;
}

void createMeshes()
{
	const std::vector<Mesh::Vertex> rectVerts = {
		{{ 1.0f, 1.0f, 0.0f }},
		{{ -1.0f, 1.0f, 0.0f }},
		{{ -1.0f, -1.0f, 0.0f }},
		{{ 1.0f, -1.0f, 0.0f }},
	};

	const std::vector<unsigned short> rectIndices =
	{
		0,1,2,2,3,0
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
