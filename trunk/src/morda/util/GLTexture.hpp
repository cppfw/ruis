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

#pragma once

#include "../config.hpp"

#ifdef M_MORDA_OGLES2
#	include <GLES2/gl2.h>
#else
#	include <GL/glew.h>
#endif

#include <ting/debug.hpp>

#include "Vector2.hpp"

#include "Image.hpp"



namespace morda{



class GLTexture {
	//no copying
	GLTexture(const GLTexture& tex);

	//no assigning
	GLTexture& operator=(const GLTexture& tex);

	ting::Inited<GLuint, 0> tex;

	morda::Vec2f dim;

	void Constructor(const Image& image, GLint minFilter, GLint magFilter);
public:
	GLTexture(const Image& image, GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR){
		this->Constructor(image, minFilter, magFilter);
	}

	GLTexture(){}

	~GLTexture()throw(){
		this->Destructor();
	}

	void Init(const Image& image, GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR){
		if(this->tex != 0){
			this->Destructor();
		}

		this->Constructor(image, minFilter, magFilter);
	}

	inline void Bind()const{
		ASSERT(glGetError() == GL_NO_ERROR)
		glBindTexture(GL_TEXTURE_2D, this->tex);
		ASSERT(glGetError() == GL_NO_ERROR)
	}
	
	inline void Bind(unsigned texUnitNum)const{
		ASSERT(glGetError() == GL_NO_ERROR)
		glActiveTexture(GL_TEXTURE0 + texUnitNum);
		ASSERT(glGetError() == GL_NO_ERROR)
		glBindTexture(GL_TEXTURE_2D, this->tex);
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline morda::Vec2f Dim()const throw(){
		return this->dim;
	}

private:
	void Destructor()throw(){
		glDeleteTextures(1, &this->tex);
	}
};



}//~namespace
