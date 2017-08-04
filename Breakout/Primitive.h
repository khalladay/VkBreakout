#pragma once
#include "stdafx.h"


namespace Primitive
{
	struct PrimitiveUniformObject
	{
		glm::mat4 model;
		glm::vec4 color;
	};


	struct PrimitiveInstance;

	int newPrimitive(int meshHdl);
	void destroyPrimitive(int handle);

	void setPrimScale(int hdl, glm::vec3 scale);
	void setPrimPos(int hdl, glm::vec3 pos);
	void setPrimCol(int hdl, glm::vec4 col);

	glm::vec3 getPrimPos(int hdl);
	glm::vec3 getPrimScale(int hdl);

	void destroyAllPrimitives();

	void submitPrimitives();
}

