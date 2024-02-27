#include "FrameBuffer.h"
#include <cstddef>
#include <stdio.h>


namespace xoxo
{
	Framebuffer::Framebuffer()
	{
		colorBuffer[0] = 0;
		depthBuffer = 0;
		width = 0;
		height = 0;
		fbo = 0;
	}

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

	Framebuffer createDepthbuffer(unsigned int depthWidth , unsigned int depthHeight , int depthValue)
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

		return Framebuffer(shadowFBO, 0, shadowMap, depthWidth, depthHeight);
	}

	Framebuffer createGBuffer(unsigned int width, unsigned int height)
	{
		Framebuffer framebuffer;

		framebuffer.width = width;
		framebuffer.height = height;

		glCreateBuffers(1, &framebuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

		int formats[3] = {
			GL_RGB32F,
			GL_RGB16F,
			GL_RGB16F
		};

		for (int i = 0; i < 3; i++)
		{
			glGenTextures(1, &framebuffer.colorBuffer[i]);
			glBindTexture(GL_TEXTURE_2D, framebuffer.colorBuffer[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, formats[i], width, height);
			//Clamp to border so we don't wrap when sampling for post processing
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//Attach each texture to a different slot.
			//GL_COLOR_ATTACHMENT0 + 1 = GL_COLOR_ATTACHMENT1, etc
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, framebuffer.colorBuffer[i], 0);
		}

		const GLenum drawBuffers[3] = {
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
		};

		glDrawBuffers(3, drawBuffers);

		glGenTextures(1, &framebuffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, framebuffer.depthBuffer);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer.depthBuffer, 0);

		//TODO check for completeness
		GLenum fboCompleteness = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboCompleteness != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Framebuffer Incomplete: %d", fboCompleteness);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return framebuffer;
	}
}