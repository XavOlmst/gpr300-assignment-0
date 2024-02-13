#include "FrameBuffer.h"
#include <cstddef>


namespace xoxo
{
	Framebuffer::Framebuffer()
	{
		colorBuffer = 0;
		depthBuffer = 0;
		width = 0;
		height = 0;
		fbo = 0;
	}

	Framebuffer::Framebuffer(unsigned int fbo, unsigned int colorBuffers, unsigned int width, unsigned int height, unsigned int depthBuffer = 0)
	{
		colorBuffer = colorBuffers;
		this->depthBuffer = depthBuffer;
		this->height = height;
		this->width = width;
		this->fbo = fbo;
	}

	Framebuffer createFramebuffer(unsigned int screenWidth, unsigned int screenHeight, int colorFormat)
	{
		unsigned int fbo, colorBuffer;

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glGenTextures(1, &colorBuffer);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, colorFormat, screenWidth, screenHeight);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBuffer, 0);

		unsigned int depthBuffer;

		glGenTextures(1, &depthBuffer);
		glBindTexture(GL_TEXTURE_2D, depthBuffer);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, screenWidth, screenHeight);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);


		return Framebuffer(fbo, colorBuffer, screenWidth, screenHeight, depthBuffer);
	}

	Framebuffer createDepthbuffer(unsigned int depthWidth , unsigned int depthHeight , int depthValue )
	{
		unsigned int shadowFBO, shadowMap;

		glGenFramebuffers(1, &shadowFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

		glGenTextures(1, &shadowMap);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glTexStorage2D(GL_TEXTURE_2D, 1, depthValue, depthWidth, depthHeight);



		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		float boarderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, boarderColor);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);



		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return Framebuffer(shadowFBO, 0, depthWidth, depthHeight, shadowMap);
	}
}