#pragma once
#include "Camera.h"

//libs
#include <vulkan/vulkan.h>

namespace lve
{
	struct FrameInfo
	{
		int frameIndex{};
		float frameTime{};
		VkCommandBuffer commandBuffer{};
		Camera& camera;
	};
}