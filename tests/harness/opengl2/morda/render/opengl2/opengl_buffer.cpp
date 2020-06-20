#include "opengl_buffer.hpp"

#include "util.hpp"

using namespace morda::render_opengl2;

opengl_buffer::opengl_buffer() :
		buffer([]() -> GLuint{
				GLuint ret;
				glGenBuffers(1, &ret);
				assertOpenGLNoError();
				return ret;
			}())
{}

opengl_buffer::~opengl_buffer()noexcept{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	assertOpenGLNoError();
	glDeleteBuffers(1, &this->buffer);
	assertOpenGLNoError();
}
