#include "opengl_buffer.hpp"

#include "OpenGL2_util.hpp"

namespace{
inline GLuint createGLBuffer(){
	GLuint ret;
	glGenBuffers(1, &ret);
	assertOpenGLNoError();
	return ret;
}
}

opengl_buffer::opengl_buffer() :
		buffer(createGLBuffer())
{
}


opengl_buffer::~opengl_buffer()noexcept{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	assertOpenGLNoError();
	glDeleteBuffers(1, &this->buffer);
	assertOpenGLNoError();
}
