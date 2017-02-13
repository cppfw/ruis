#include <utki/config.hpp>

#include "OpenGLES2FrameBuffer.hpp"
#include "OpenGLES2Texture2D.hpp"

#include "OpenGLES2_util.hpp"

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace mordaren;

OpenGLES2FrameBuffer::OpenGLES2FrameBuffer(std::shared_ptr<morda::Texture2D> color) :
		morda::FrameBuffer(std::move(color))
{
	glGenFramebuffers(1, &this->fbo);
	assertOpenGLNoError();
	
	GLint oldFb;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
	
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
	assertOpenGLNoError();
	
	ASSERT(dynamic_cast<OpenGLES2Texture2D*>(this->color.operator->()))
	auto& tex = static_cast<OpenGLES2Texture2D&>(*this->color);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.tex, 0);
	assertOpenGLNoError();
	
	//Check for completeness
#ifdef DEBUG
	{
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		assertOpenGLNoError();
		ASSERT(status == GL_FRAMEBUFFER_COMPLETE)
	}
#endif
	
	glBindFramebuffer(GL_FRAMEBUFFER, oldFb);
	assertOpenGLNoError();
}


OpenGLES2FrameBuffer::~OpenGLES2FrameBuffer()noexcept{
	glDeleteFramebuffers(1, &this->fbo);
	assertOpenGLNoError();
}
