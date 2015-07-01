/* The MIT License:

Copyright (c) 2015 Ivan Gagis <igagis@gmail.com>

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

#include <ting/Singleton.hpp>

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



class Render : ting::Singleton<Render>{
	friend class Shader;
	
public:
	enum class EMode{
		TRIANGLES = GL_TRIANGLES,
		TRIANGLE_FAN = GL_TRIANGLE_FAN,
		LINE_LOOP = GL_LINE_LOOP
	};
	
private:
	
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

	void DrawArrays(GLenum mode, unsigned numElements){
		glDrawArrays(mode, 0, numElements);
		AssertOpenGLNoError();
	}
	
	
};



}
