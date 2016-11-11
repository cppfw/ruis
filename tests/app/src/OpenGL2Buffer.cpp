#include "OpenGL2Buffer.hpp"

#include "OpenGL2_util.hpp"

namespace{
inline GLuint createGLBuffer(){
	GLuint ret;
	glGenBuffers(1, &ret);
	return ret;
}
}

OpenGL2Buffer::OpenGL2Buffer() :
		buffer(createGLBuffer())
{
	AssertOpenGLNoError();
}


OpenGL2Buffer::~OpenGL2Buffer()noexcept{
	glDeleteBuffers(1, &this->buffer);
	AssertOpenGLNoError();
}
