#include "OpenGL2FrameBuffer.hpp"
#include "OpenGL2Texture2D.hpp"

#include "OpenGL2_util.hpp"

#include <GL/glew.h>

using namespace mordaren;

OpenGL2FrameBuffer::OpenGL2FrameBuffer(std::shared_ptr<morda::texture_2d> color) :
		morda::frame_buffer(std::move(color))
{
	glGenFramebuffers(1, &this->fbo);
	assertOpenGLNoError();
	
	GLint oldFb;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
	
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
	assertOpenGLNoError();
	
	ASSERT(dynamic_cast<OpenGL2Texture2D*>(this->color.operator->()))
	auto& tex = static_cast<OpenGL2Texture2D&>(*this->color);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.tex, 0);
	assertOpenGLNoError();
	
	// check for completeness
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


OpenGL2FrameBuffer::~OpenGL2FrameBuffer()noexcept{
	glDeleteFramebuffers(1, &this->fbo);
	assertOpenGLNoError();
}
