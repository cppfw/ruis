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

#include "../render/Render.hpp"

#include "../util/Matrix4.hpp"
#include "../util/Vector3.hpp"
#include "../util/Vector2.hpp"



//TODO: remove
#include "../config.hpp"

#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGL
#	include <GL/glew.h>

#	if M_OS == M_OS_LINUX
#		include <GL/glx.h>
#	else
#		include <ting/windows.hpp>
#	endif

#elif M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES
#		include <GLES2/gl2.h>
#		include <EGL/egl.h>
#else
#	error "unknown render API"
#endif






namespace morda{



/**
 * @brief Abstract shader.
 * Each shader has at least matrix uniform.
 * Matrix uniform should be 'mat4' and named 'matrix'.
 */
class Shader{
	friend class Render;
	
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
	
	struct ProgramWrapper{
		ShaderWrapper vertexShader;
		ShaderWrapper fragmentShader;
		GLuint p;
		ProgramWrapper(const char* vertexShaderCode, const char* fragmentShaderCode);
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

	void renderArrays(Render::EMode mode, unsigned numElements){
		ASSERT(this->IsBound())
		Render::renderArrays(mode, numElements);
	}
	
	void renderElements(Render::EMode mode, ting::Buffer<const std::uint16_t> i){
		ASSERT(this->IsBound())
		Render::renderElements(mode, i);
	}
	
public:
	
	
	virtual ~Shader()NOEXCEPT{}

	void Bind(){//TODO: make private
		if(this->IsBound()){
			return;
		}
		
		Render::bindShader(*this);
		boundShader = this;
	}

	bool IsBound()const NOEXCEPT{//TODO: make private
		return this == boundShader;
	}
	
	void SetMatrix(const morda::Matr4f &m){
		ASSERT(this->IsBound())
		glUniformMatrix4fv(this->matrixUniform, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&m));
		AssertOpenGLNoError();
	}
	
	//TODO: remove
	static void AssertOpenGLNoError(){

	}
};//~class Shader



}//~namespace
