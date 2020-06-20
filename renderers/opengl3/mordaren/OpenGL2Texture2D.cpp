#include "texture_2d.hpp"

#include "OpenGL2_util.hpp"

texture_2d::texture_2d(kolme::Vec2f dim) :
		morda::Texture2D(dim)
{
	glGenTextures(1, &this->tex);
	assertOpenGLNoError();
	ASSERT(this->tex != 0)
}


texture_2d::~texture_2d()noexcept{
	glDeleteTextures(1, &this->tex);
}

void texture_2d::bind(unsigned unitNum) const {
	glActiveTexture(GL_TEXTURE0 + unitNum);
	assertOpenGLNoError();
	glBindTexture(GL_TEXTURE_2D, this->tex);
	assertOpenGLNoError();
}
