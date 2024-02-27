#pragma once
#include "../ew/external/glad.h"

namespace xoxo
{
	struct Framebuffer
	{
		Framebuffer(unsigned int fbo, unsigned int colorBuffer, unsigned int depthBuffer, unsigned int width, unsigned int height);
		Framebuffer();

		unsigned int fbo;
		unsigned int colorBuffer[8];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};

	Framebuffer createFramebuffer(unsigned int screenWidth, unsigned int screenHeight, int colorFormat);
	Framebuffer createDepthbuffer(unsigned int depthWidth = 2048, unsigned int depthHeight = 2048, int depthValue = GL_DEPTH_COMPONENT16);
	Framebuffer createGBuffer(unsigned int width, unsigned int height);
}