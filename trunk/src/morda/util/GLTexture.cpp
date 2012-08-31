/* The MIT License:

Copyright (c) 2012 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */



#include "GLTexture.hpp"



using namespace morda;



void GLTexture::Init(const Image& image, GLint minFilter, GLint magFilter){
	if(this->tex != 0){
		this->Destroy();
	}

	this->dim = morda::Vec2f(image.Width(), image.Height());

	glGenTextures(1, &this->tex);
	ASSERT(glGetError() == GL_NO_ERROR)
	ASSERT(this->tex != GL_INVALID_VALUE)
	ASSERT(this->tex != 0)
	glActiveTexture(GL_TEXTURE0);
	ASSERT(glGetError() == GL_NO_ERROR)
	glBindTexture(GL_TEXTURE_2D, this->tex);
	ASSERT(glGetError() == GL_NO_ERROR)

	GLint internalFormat;
	switch(image.NumChannels()){
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
		default:
			ASSERT(false)
			break;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	ASSERT(glGetError() == GL_NO_ERROR)

	glTexImage2D(
			GL_TEXTURE_2D,
			0,//0th level, no mipmaps
			internalFormat, //internal format
			image.Width(),
			image.Height(),
			0,//border, should be 0!
			internalFormat, //format of the texel data
			GL_UNSIGNED_BYTE,
			image.Buf().Begin()
		);
	ASSERT(glGetError() == GL_NO_ERROR)

	//NOTE: on OpenGL ES 2 it is necessary to set the filter parameters
	//      for every texture!!! Otherwise it may not work!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	ASSERT(glGetError() == GL_NO_ERROR)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	ASSERT(glGetError() == GL_NO_ERROR)
}
