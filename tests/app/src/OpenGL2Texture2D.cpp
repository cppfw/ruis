#include "OpenGL2Texture2D.hpp"

#include "OpenGL2_util.hpp"

OpenGL2Texture2D::OpenGL2Texture2D(kolme::Vec2f dim) :
		morda::Texture2D(dim)
{
	glGenTextures(1, &this->tex);
	AssertOpenGLNoError();
	ASSERT(this->tex != 0)
}


OpenGL2Texture2D::~OpenGL2Texture2D()noexcept{
	glDeleteTextures(1, &this->tex);
}

void OpenGL2Texture2D::bind(unsigned unitNum) const {
	glActiveTexture(GL_TEXTURE0 + unitNum);
	AssertOpenGLNoError();
	glBindTexture(GL_TEXTURE_2D, this->tex);
	AssertOpenGLNoError();
}
