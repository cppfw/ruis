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

#include <GL/glew.h>

#include <tride/Vector3.hpp>

#include <ting/types.hpp>
#include <ting/Singleton.hpp>


class Shader{
protected:
	ting::Inited<GLuint, 0> vertexShader;
	ting::Inited<GLuint, 0> fragmentShader;
	ting::Inited<GLuint, 0> program;

	GLuint positionAttr;

	GLuint matrixUniform;
public:
	Shader(){}

	~Shader()throw();

	inline void Bind(){
		ASSERT(this->program != 0)
		glUseProgram(this->program);
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline void SetMatrix(const tride::Matr4f &m){
		glUniformMatrix4fv(this->matrixUniform, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&m));
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline void SetPositionPointer(const tride::Vec3f *p){
		ASSERT(p)
		glVertexAttribPointer(this->positionAttr, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*>(p));
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline void SetPositionPointer(const tride::Vec2f *p){
		ASSERT(p)
		glVertexAttribPointer(this->positionAttr, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*>(p));
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline void EnablePositionPointer(){
		glEnableVertexAttribArray(this->positionAttr);
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline void DisablePositionPointer(){
		glDisableVertexAttribArray(this->positionAttr);
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline void DrawArrays(GLenum mode, unsigned numElements){
		glDrawArrays(mode, 0, numElements);
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	void DrawQuad(GLenum mode = GL_TRIANGLE_FAN);

	void DrawQuad01(GLenum mode = GL_TRIANGLE_FAN);

protected:
	//return true if not compiled
	static bool CheckForCompileErrors(GLuint shader);

	//return true if not linked
	static bool CheckForLinkErrors(GLuint program);
};//~class Shader
