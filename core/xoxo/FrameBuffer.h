#pragma once
#include "../ew/external/glad.h"

namespace xoxo
{
	struct Framebuffer
	{
		Framebuffer(unsigned int fbo, unsigned int colorBuffer[8], unsigned int depthBuffer, unsigned int width, unsigned int height);

		unsigned int fbo;
		unsigned int colorBuffer[8];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};

	Framebuffer createFramebuffer(unsigned int screenWidth, unsigned int screenHeight, int colorFormat);
}