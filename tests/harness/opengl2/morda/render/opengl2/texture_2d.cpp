#include "texture_2d.hpp"

#include "util.hpp"

using namespace morda::render_opengl2;

texture_2d::texture_2d(r4::vector2<float> dims) :
		morda::texture_2d(dims)
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
