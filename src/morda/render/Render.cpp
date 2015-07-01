#include "Render.hpp"

#include <ting/debug.hpp>

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


#include "../shaders/Shader.hpp"


using namespace morda;


namespace{

#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGL

inline static void AssertOpenGLNoError(){
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
	



GLenum modeMap[] = {
	GL_TRIANGLES,			//TRIANGLES
	GL_TRIANGLE_FAN,		//TRIANGLE_FAN
	GL_LINE_LOOP			//LINE_LOOP
};

#endif
}



void Render::renderArrays(EMode mode, unsigned numElements) {
	GLenum m = modeMap[unsigned(mode)];
	
	glDrawArrays(m, 0, numElements);
	AssertOpenGLNoError();
}



void Render::renderElements(EMode mode, const ting::Buffer<const std::uint16_t>& i) {
	GLenum m = modeMap[unsigned(mode)];
	
	glDrawElements(m, i.size(), GL_UNSIGNED_SHORT, &*i.begin());
	AssertOpenGLNoError();
}

void Render::bindShader(Shader& s) {
	glUseProgram(s.program.p);
	AssertOpenGLNoError();
}


