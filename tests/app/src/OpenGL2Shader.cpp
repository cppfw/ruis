#include "OpenGL2Shader.hpp"

#include <GL/glew.h>

#include <utki/debug.hpp>
#include <utki/Exc.hpp>

#include <vector>


OpenGL2Shader::OpenGL2Shader(const char* vertexShaderCode, const char* fragmentShaderCode) :
		program(vertexShaderCode, fragmentShaderCode)
{
	this->matrixUniform = this->getUniform("matrix");
}

GLint OpenGL2Shader::getUniform(const char* n) {
	GLint ret = glGetUniformLocation(this->program.p, n);
	if(ret < 0){
		throw utki::Exc("No uniform found in the shader program");
	}
	return ret;
}

GLint OpenGL2Shader::getAttribute(const char* n) {
	GLint ret = glGetAttribLocation(this->program.p, n);
	if(ret < 0){
		std::stringstream ss;
		ss << "No attribute found in the shader program: " << n;
		throw utki::Exc(ss.str());
	}
	return ret;
}

void OpenGL2Shader::setVertexAttribArray(GLint id, const kolme::Vec3f* a) {
	glEnableVertexAttribArray(id);
//	AssertOpenGLNoError();
	ASSERT(a)
	glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*> (a));
//	AssertOpenGLNoError();
}

void OpenGL2Shader::setVertexAttribArray(GLint id, const kolme::Vec2f* a) {
	glEnableVertexAttribArray(id);
//	AssertOpenGLNoError();
	ASSERT(a)
	glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*> (a));
//	AssertOpenGLNoError();
}

namespace{
GLenum modeMap[] = {
	GL_TRIANGLES,			//TRIANGLES
	GL_TRIANGLE_FAN,		//TRIANGLE_FAN
	GL_LINE_LOOP,			//LINE_LOOP
	GL_TRIANGLE_STRIP		//TRIANGLE_STRIP
};
}


void OpenGL2Shader::renderElements(morda::Shader_n::Mode_e mode, const utki::Buf<std::uint16_t>& i) {
	GLenum m = modeMap[unsigned(mode)];

	glDrawElements(m, GLsizei(i.size()), GL_UNSIGNED_SHORT, &*i.begin());
//	AssertOpenGLNoError();
}
