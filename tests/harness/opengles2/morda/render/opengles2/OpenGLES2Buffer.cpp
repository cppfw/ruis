#include "OpenGLES2Buffer.hpp"

#include "OpenGLES2_util.hpp"

using namespace morda;

namespace{
inline GLuint createGLBuffer(){
	GLuint ret;
	glGenBuffers(1, &ret);
	assertOpenGLNoError();
	return ret;
}
}

OpenGLES2Buffer::OpenGLES2Buffer() :
		buffer(createGLBuffer())
{
}


OpenGLES2Buffer::~OpenGLES2Buffer()noexcept{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	assertOpenGLNoError();
	glDeleteBuffers(1, &this->buffer);
	assertOpenGLNoError();
}
