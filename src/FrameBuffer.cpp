
#include <engine/opengl/FrameBuffer.hpp>

#include <glad/glad.h>

using namespace Engine::Internal;


// TODO-fix : get max color attachment and limit it
FrameBuffer::FrameBuffer (int32_t width, int32_t height):
	mWidth  {width},
	mHeight {height}	
	{

	return;

	mColorAttachments.resize (1);

	glGenFramebuffers (1, &mFrameBuffer);
	glBindFramebuffer (GL_FRAMEBUFFER, mFrameBuffer);

	glGenTextures (mColorAttachments.size (), mColorAttachments.data ());
	int idx = 0;
	for (auto & attachement : mColorAttachments) {
		glBindTexture (GL_TEXTURE_2D, attachement);
		glTexImage2D  (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture (GL_TEXTURE_2D, 0);	
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx++, GL_TEXTURE_2D, attachement, 0);
	}
	
	glGenRenderbuffers (1, &mRenderBuffer);
	glBindRenderbuffer (GL_RENDERBUFFER, mRenderBuffer);
	glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer (GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);

	if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf ("SceneFrameBuffer is not complete :(\n");
	}
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer () {
	return;
	printf ("framebuffer deleted: %u\n", mFrameBuffer);
	glDeleteFramebuffers	(1, &mFrameBuffer);
	glDeleteRenderbuffers	(1, &mRenderBuffer);
	glDeleteTextures	(mColorAttachments.size (), mColorAttachments.data ());
}

uint32_t FrameBuffer::AddColorAttachment () {
	return 0;
	mColorAttachments.emplace_back (0);

	glGenTextures   (1, &mColorAttachments.back ());
	glBindTexture   (GL_TEXTURE_2D, mColorAttachments.back ());
	glTexImage2D    (GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture   (GL_TEXTURE_2D, 0);	
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + mColorAttachments.size (), GL_TEXTURE_2D, mColorAttachments.back (), 0);
}

uint32_t FrameBuffer::Get () const {
	return (mFrameBuffer);
}

uint32_t FrameBuffer::GetColorAttachment (uint32_t binding) const {
	return (mColorAttachments.at (binding));
}