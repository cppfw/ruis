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
#	include <GL/glx.h>
#endif

#include "../util/Vector3.hpp"

#include <ting/types.hpp>
#include <ting/Singleton.hpp>



namespace morda{



/**
 * @brief Abstract shader.
 * Each shader has at least matrix uniform and vertex position attribute.
 * Matrix uniform should be 'mat4' and named 'matrix'.
 * Vertex position attribute should be 'vec4' and named 'vertex'.
 */
class Shader{
	struct ShaderWrapper{
		GLuint s;
		ShaderWrapper(const char* code, GLenum type);
		inline ~ShaderWrapper()throw(){
			glDeleteShader(this->s);
		}
		
		//return true if not compiled
		static bool CheckForCompileErrors(GLuint shader);
	};
	
	ShaderWrapper vertexShader;
	ShaderWrapper fragmentShader;
	
	struct ProgramWrapper{
		GLuint p;
		ProgramWrapper(GLuint vertex, GLuint fragment);
		inline ~ProgramWrapper()throw(){
			glDeleteProgram(this->p);
		}
		
		//return true if not linked
		static bool CheckForLinkErrors(GLuint program);
	};
	
	ProgramWrapper program;

	GLint positionAttr;

	GLint matrixUniform;
	
protected:
	inline GLint GetAttribute(const char* n){
		GLint ret = glGetAttribLocation(this->program.p, n);
		if(ret < 0){
			throw ting::Exc("No attribute found in the shader program");
		}
		return ret;
	}
	inline GLint GetUniform(const char* n){
		GLint ret = glGetUniformLocation(this->program.p, n);
		if(ret < 0){
			throw ting::Exc("No uniform found in the shader program");
		}
		return ret;
	}
public:
	Shader(const char* vertexShaderCode, const char* fragmentShaderCode);

	virtual ~Shader()throw(){}

	inline void Bind(){
		glUseProgram(this->program.p);
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline void SetMatrix(const morda::Matr4f &m){
		glUniformMatrix4fv(this->matrixUniform, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&m));
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline void SetPositionPointer(const morda::Vec3f *p){
		ASSERT(p)
		glVertexAttribPointer(this->positionAttr, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*>(p));
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	inline void SetPositionPointer(const morda::Vec2f *p){
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

	void DrawQuad();

	void DrawQuad01(GLint type = GL_TRIANGLE_FAN);
};//~class Shader



}//~namespace
