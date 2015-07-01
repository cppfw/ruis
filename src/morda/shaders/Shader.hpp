/* The MIT License:

Copyright (c) 2012-2015 Ivan Gagis <igagis@gmail.com>

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

#include <ting/config.hpp>
#include <ting/types.hpp>
#include <ting/Singleton.hpp>
#include <ting/Buffer.hpp>

#include "../config.hpp"

#if M_OS == M_OS_LINUX
#	ifdef M_MORDA_OGLES2
#		include <GLES2/gl2.h>
#		include <EGL/egl.h>
#	else
#		include <GL/glew.h>
#		include <GL/glx.h>
#	endif
#elif M_OS == M_OS_WINDOWS
#	include <GL/glew.h>
#elif M_OS == M_OS_MACOSX
#	include <GL/glew.h>
#else
#	error "unknown os"
#endif

#include "../util/Matrix4.hpp"
#include "../util/Vector3.hpp"
#include "../util/Vector2.hpp"



namespace morda{



/**
 * @brief Abstract shader.
 * Each shader has at least matrix uniform.
 * Matrix uniform should be 'mat4' and named 'matrix'.
 */
class Shader{
	static Shader* boundShader;
	
	struct ShaderWrapper{
		GLuint s;
		ShaderWrapper(const char* code, GLenum type);
		~ShaderWrapper()NOEXCEPT{
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
		~ProgramWrapper()NOEXCEPT{
			glDeleteProgram(this->p);
		}
		
		//return true if not linked
		static bool CheckForLinkErrors(GLuint program);
	};
	
	ProgramWrapper program;

	GLint matrixUniform;
	
protected:
	GLint GetAttribute(const char* n){
		GLint ret = glGetAttribLocation(this->program.p, n);
		if(ret < 0){
			std::stringstream ss;
			ss << "No attribute found in the shader program: " << n;
			throw ting::Exc(ss.str());
		}
		return ret;
	}
	
	GLint GetUniform(const char* n){
		GLint ret = glGetUniformLocation(this->program.p, n);
		if(ret < 0){
			throw ting::Exc("No uniform found in the shader program");
		}
		return ret;
	}

	Shader(const char* vertexShaderCode = nullptr, const char* fragmentShaderCode = nullptr);

	void DrawArrays(GLenum mode, unsigned numElements){
		ASSERT(this->IsBound())
		glDrawArrays(mode, 0, numElements);
		AssertOpenGLNoError();
	}
	
	void DrawElements(GLenum mode, ting::Buffer<const std::uint16_t> i){
		ASSERT(this->IsBound())
		glDrawElements(mode, i.size(), GL_UNSIGNED_SHORT, &*i.begin());
		AssertOpenGLNoError();
	}
	
public:
	enum class EMode{
		TRIANGLES = GL_TRIANGLES,
		TRIANGLE_FAN = GL_TRIANGLE_FAN,
		LINE_LOOP = GL_LINE_LOOP
	};
	
	virtual ~Shader()NOEXCEPT{}

	void Bind(){
		if(this->IsBound()){
			return;
		}
		
		glUseProgram(this->program.p);
		AssertOpenGLNoError();
		boundShader = this;
	}

	bool IsBound()const NOEXCEPT{
		return this == boundShader;
	}
	
	void SetMatrix(const morda::Matr4f &m){
		ASSERT(this->IsBound())
		glUniformMatrix4fv(this->matrixUniform, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&m));
		AssertOpenGLNoError();
	}
	
	static void AssertOpenGLNoError(){
#ifdef DEBUG
		GLenum error = glGetError();
		switch(error){
			case GL_NO_ERROR:
				return;
			case GL_INVALID_ENUM:
				ASSERT_INFO(false, "OpenGL error: GL_INVALID_ENUM")
				break;
			case GL_INVALID_VALUE:
				ASSERT_INFO(false, "OpenGL error: GL_INVALID_VALUE")
				break;
			case GL_INVALID_OPERATION:
				ASSERT_INFO(false, "OpenGL error: GL_INVALID_OPERATION")
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				ASSERT_INFO(false, "OpenGL error: GL_INVALID_FRAMEBUFFER_OPERATION")
				break;
			case GL_OUT_OF_MEMORY:
				ASSERT_INFO(false, "OpenGL error: GL_OUT_OF_MEMORY")
				break;
			default:
				ASSERT_INFO(false, "Unknown OpenGL error, code = " << int(error))
				break;
		}
#endif
	}
};//~class Shader



}//~namespace
