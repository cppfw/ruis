#include "OpenGL2Buffer.hpp"

#include "OpenGL2_util.hpp"

namespace{
inline GLuint createGLBuffer(){
	GLuint ret;
	glGenBuffers(1, &ret);
	AssertOpenGLNoError();
	return ret;
}
}

OpenGL2Buffer::OpenGL2Buffer() :
		buffer(createGLBuffer())
{
}


OpenGL2Buffer::~OpenGL2Buffer()noexcept{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	AssertOpenGLNoError();
	glDeleteBuffers(1, &this->buffer);
	AssertOpenGLNoError();
}
