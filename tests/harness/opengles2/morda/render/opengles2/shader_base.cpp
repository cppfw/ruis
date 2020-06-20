#include <utki/config.hpp>
#include <utki/debug.hpp>

#include <vector>

#include "shader_base.hpp"
#include "util.hpp"
#include "vertex_array.hpp"
#include "index_buffer.hpp"
#include "vertex_buffer.hpp"

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace morda::render_opengles2;

const shader_base* shader_base::boundShader = nullptr;

GLenum shader_base::modeMap[] = {
	GL_TRIANGLES,			// TRIANGLES
	GL_TRIANGLE_FAN,		// TRIANGLE_FAN
	GL_LINE_LOOP,			// LINE_LOOP
	GL_TRIANGLE_STRIP		// TRIANGLE_STRIP
};

namespace{
//return true if not compiled
bool checkForCompileErrors(GLuint shader) {
	GLint value = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
	assertOpenGLNoError();
	if (value == 0) { //if not compiled
		GLint logLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		assertOpenGLNoError();
		if (logLen > 1) {//1 char is a terminating 0
			std::vector<char> log(logLen);
			GLint len;
			glGetShaderInfoLog(shader, GLsizei(log.size()), &len, &*log.begin());
			assertOpenGLNoError();
			TRACE( << "===Compile log===\n" << &*log.begin() << std::endl)
		} else {
			TRACE( << "Shader compile log is empty" << std::endl)
		}
		return true;
	}
	return false;
}

//return true if not linked
bool checkForLinkErrors(GLuint program){
	GLint value = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &value);
	assertOpenGLNoError();
	if(value == 0){ //if not linked
		GLint logLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		assertOpenGLNoError();
		if(logLen > 1){ //1 is for terminating 0 character.
			std::vector<char> log(logLen);
			GLint len;
			glGetProgramInfoLog(program, GLsizei(log.size()), &len, &*log.begin());
			assertOpenGLNoError();
			TRACE(<< "===Link log===\n" << &*log.begin() << std::endl)
		}
		return true;
	}
	return false;
}
}

shader_wrapper::shader_wrapper(const char* code, GLenum type) {
	this->s = glCreateShader(type);
	assertOpenGLNoError();

	if (this->s == 0) {
		throw std::runtime_error("glCreateShader() failed");
	}

	const char* c = code;

	glShaderSource(this->s, 1, &c, 0);
	assertOpenGLNoError();
	glCompileShader(this->s);
	assertOpenGLNoError();
	if(checkForCompileErrors(this->s)) {
		TRACE( << "Error while compiling:\n" << c << std::endl)
		glDeleteShader(this->s);
		assertOpenGLNoError();
		throw std::logic_error("Error compiling shader");
	}
}

program_wrapper::program_wrapper(const char* vertexShaderCode, const char* fragmentShaderCode) :
		vertexShader(vertexShaderCode, GL_VERTEX_SHADER),
		fragmentShader(fragmentShaderCode, GL_FRAGMENT_SHADER)
{
	this->p = glCreateProgram();
	assertOpenGLNoError();
	glAttachShader(this->p, vertexShader.s);
	assertOpenGLNoError();
	glAttachShader(this->p, fragmentShader.s);
	assertOpenGLNoError();
	
	GLint maxAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
	assertOpenGLNoError();
	ASSERT(maxAttribs >= 0)
	
	for(GLuint i = 0; i < GLuint(maxAttribs); ++i){
		std::stringstream ss;
		ss << "a" << i;
//		TRACE(<< ss.str() << std::endl)
		glBindAttribLocation(this->p, i, ss.str().c_str());
		assertOpenGLNoError();
	}
	
	glLinkProgram(this->p);
	assertOpenGLNoError();
	if (checkForLinkErrors(this->p)) {
		TRACE( << "Error while linking shader program" << vertexShaderCode << std::endl << fragmentShaderCode << std::endl)
		glDeleteProgram(this->p);
		assertOpenGLNoError();
		throw std::logic_error("Error linking shader program");
	}
}

shader_base::shader_base(const char* vertexShaderCode, const char* fragmentShaderCode) :
		program(vertexShaderCode, fragmentShaderCode),
		matrixUniform(this->getUniform("matrix"))
{
}

GLint shader_base::getUniform(const char* n) {
	GLint ret = glGetUniformLocation(this->program.p, n);
	assertOpenGLNoError();
	if(ret < 0){
		throw std::logic_error("No uniform found in the shader program");
	}
	return ret;
}

void shader_base::render(const r4::mat4f& m, const morda::vertex_array& va)const{
	ASSERT(this->isBound())
	
	ASSERT(dynamic_cast<const index_buffer*>(va.indices.get()))
	auto& ivbo = static_cast<const index_buffer&>(*va.indices);
	
	this->setMatrix(m);
	
	for(unsigned i = 0; i != va.buffers.size(); ++i){
		ASSERT(dynamic_cast<vertex_buffer*>(va.buffers[i].get()))
		auto& vbo = static_cast<vertex_buffer&>(*va.buffers[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo.buffer);
		assertOpenGLNoError();
		
//		TRACE(<< "vbo.numComponents = " << vbo.numComponents << " vbo.type = " << vbo.type << std::endl)
		
		glVertexAttribPointer(i, vbo.numComponents, vbo.type, GL_FALSE, 0, nullptr);
		assertOpenGLNoError();
		
		glEnableVertexAttribArray(i);
		assertOpenGLNoError();
	}
	
	{
		ASSERT(dynamic_cast<index_buffer*>(va.indices.get()))
		auto& ivbo = static_cast<index_buffer&>(*va.indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ivbo.buffer);
		assertOpenGLNoError();
	}
	

//	TRACE(<< "ivbo.elementsCount = " << ivbo.elementsCount << " ivbo.elementType = " << ivbo.elementType << std::endl)
	
	glDrawElements(modeToGLMode(va.rendering_mode), ivbo.elementsCount, ivbo.elementType, nullptr);
	assertOpenGLNoError();
}

