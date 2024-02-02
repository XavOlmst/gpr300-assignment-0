#include "FrameBuffer.h"


namespace xoxo
{
	Framebuffer::Framebuffer(unsigned int fbo, unsigned int colorBuffers, unsigned int depthBuffer, unsigned int width, unsigned int height)
	{
		colorBuffer[0] = colorBuffers;
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


		return Framebuffer(fbo, colorBuffer, depthBuffer, screenWidth, screenHeight);
	}
}