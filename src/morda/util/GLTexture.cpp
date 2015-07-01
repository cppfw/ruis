#include "GLTexture.hpp"



using namespace morda;




void GLTexture::Constructor(Vec2ui d, unsigned numChannels, ting::Buffer<const std::uint8_t> data, GLint minFilter, GLint magFilter) {
	this->dim = d.ConvertTo<real>();

	glGenTextures(1, &this->tex);
	ASSERT(glGetError() == GL_NO_ERROR)
	ASSERT(this->tex != 0)
	glActiveTexture(GL_TEXTURE0);
	ASSERT(glGetError() == GL_NO_ERROR)
	glBindTexture(GL_TEXTURE_2D, this->tex);
	ASSERT(glGetError() == GL_NO_ERROR)

	GLint internalFormat;
	switch(numChannels){
		default:
			ASSERT(false)
		case 1:
			internalFormat = GL_LUMINANCE;
			break;
		case 2:
			internalFormat = GL_LUMINANCE_ALPHA;
			break;
		case 3:
			internalFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA;
			break;
	}

	//we will be passing pixels to OpenGL which are 1-byte aligned.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	ASSERT(glGetError() == GL_NO_ERROR)

	glTexImage2D(
			GL_TEXTURE_2D,
			0,//0th level, no mipmaps
			internalFormat, //internal format
			d.x,
			d.y,
			0,//border, should be 0!
			internalFormat, //format of the texel data
			GL_UNSIGNED_BYTE,
			&*data.begin()
		);
	ASSERT(glGetError() == GL_NO_ERROR)

	//NOTE: on OpenGL ES 2 it is necessary to set the filter parameters
	//      for every texture!!! Otherwise it may not work!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	ASSERT(glGetError() == GL_NO_ERROR)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	ASSERT(glGetError() == GL_NO_ERROR)
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
