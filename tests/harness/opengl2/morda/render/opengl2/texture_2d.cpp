#include "texture_2d.hpp"

#include "util.hpp"

using namespace morda::render_opengl2;

OpenGL2Texture2D::OpenGL2Texture2D(r4::vec2f dims) :
		morda::texture_2d(dims)
{
	glGenTextures(1, &this->tex);
	assertOpenGLNoError();
	ASSERT(this->tex != 0)
}


OpenGL2Texture2D::~OpenGL2Texture2D()noexcept{
	glDeleteTextures(1, &this->tex);
}

void OpenGL2Texture2D::bind(unsigned unitNum) const {
	glActiveTexture(GL_TEXTURE0 + unitNum);
	assertOpenGLNoError();
	glBindTexture(GL_TEXTURE_2D, this->tex);
	assertOpenGLNoError();
}
