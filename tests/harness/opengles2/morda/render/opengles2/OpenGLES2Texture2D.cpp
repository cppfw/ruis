#include "OpenGLES2Texture2D.hpp"

#include "OpenGLES2_util.hpp"

using namespace morda::render_opengles2;

OpenGLES2Texture2D::OpenGLES2Texture2D(r4::vec2f dims) :
		texture_2d(dims)
{
	glGenTextures(1, &this->tex);
	assertOpenGLNoError();
	ASSERT(this->tex != 0)
}

OpenGLES2Texture2D::~OpenGLES2Texture2D()noexcept{
	glDeleteTextures(1, &this->tex);
}

void OpenGLES2Texture2D::bind(unsigned unitNum) const {
	glActiveTexture(GL_TEXTURE0 + unitNum);
	assertOpenGLNoError();
	glBindTexture(GL_TEXTURE_2D, this->tex);
	assertOpenGLNoError();
}
