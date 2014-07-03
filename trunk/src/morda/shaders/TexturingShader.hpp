/* The MIT License:

Copyright (c) 2012 Ivan Gagis

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


#include "Shader.hpp"


namespace morda{



/**
 * @brief Base class for texturing shader.
 * Base class for all texturing shaders. The texture coordinate attribute in the
 * shader code should be 'vec2' and have name 'textureCoord'. Texture number
 * uniform in the shader code should be 'sampler2D' and have name 'textureNumber'.
 */
class TexturingShader : virtual public Shader{

	//no copying
	TexturingShader(const TexturingShader&);
	
	//no assignment
	TexturingShader& operator=(const TexturingShader&);
	
private:
	GLuint texCoordAttr;

	GLuint texNumberUniform;

protected:
	TexturingShader() :
			Shader(0, 0)
	{
		this->texCoordAttr = this->GetAttribute("textureCoord");
		this->texNumberUniform = this->GetUniform("textureNumber");
	}

public:	
	void SetTextureNumber(unsigned i){
		glUniform1i(this->texNumberUniform, i);
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	void SetTexCoordPointer(const morda::Vec2f *p){
		ASSERT(p)
		glVertexAttribPointer(this->texCoordAttr, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*>(p));
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	void EnableTexCoordPointer(){
		glEnableVertexAttribArray(this->texCoordAttr);
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	void DisableTexCoordPointer(){
		glDisableVertexAttribArray(this->texCoordAttr);
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	void DrawQuad();
	
	void DrawQuad01();
};//~class TexturingShader



}//~namespace
