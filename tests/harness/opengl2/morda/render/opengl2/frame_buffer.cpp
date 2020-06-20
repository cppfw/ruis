#include "frame_buffer.hpp"
#include "texture_2d.hpp"

#include "util.hpp"

#include <GL/glew.h>

using namespace morda::render_opengl2;

frame_buffer::frame_buffer(std::shared_ptr<morda::texture_2d> color) :
		morda::frame_buffer(std::move(color))
{
	glGenFramebuffers(1, &this->fbo);
	assertOpenGLNoError();
	
	GLint oldFb;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
	
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
	assertOpenGLNoError();
	
	ASSERT(dynamic_cast<texture_2d*>(this->color.operator->()))
	auto& tex = static_cast<texture_2d&>(*this->color);
	
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

frame_buffer::~frame_buffer()noexcept{
	glDeleteFramebuffers(1, &this->fbo);
	assertOpenGLNoError();
}
