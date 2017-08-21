#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define APP_NAME "Vulkan Breakout"
#define SCREEN_W 1280
#define SCREEN_H 720

#define STRESS_TEST 1
#if STRESS_TEST
#define MAX_PRIMS 5060
#else
#define MAX_PRIMS 500
#endif

#define ENABLE_VK_TIMESTAMP 1
#define DEVICE_LOCAL_MEMORY 1