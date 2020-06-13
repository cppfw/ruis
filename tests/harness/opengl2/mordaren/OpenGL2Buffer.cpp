#include "OpenGL2Buffer.hpp"

#include "OpenGL2_util.hpp"

using namespace mordaren;


OpenGL2Buffer::OpenGL2Buffer() :
		buffer([]() -> GLuint{
				GLuint ret;
				glGenBuffers(1, &ret);
				assertOpenGLNoError();
				return ret;
			}())
{
}


OpenGL2Buffer::~OpenGL2Buffer()noexcept{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	assertOpenGLNoError();
	glDeleteBuffers(1, &this->buffer);
	assertOpenGLNoError();
}
