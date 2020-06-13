#pragma once

#include <utki/config.hpp>
#include <utki/debug.hpp>

#include <r4/matrix4.hpp>

#include <vector>

#include "OpenGL2_util.hpp"

#include <morda/render/vertex_array.hpp>

#include <GL/glew.h>

namespace mordaren{

struct ShaderWrapper{
	GLuint s;
	ShaderWrapper(const char* code, GLenum type);
	~ShaderWrapper()noexcept{
		glDeleteShader(this->s);
	}
};

struct ProgramWrapper{
	ShaderWrapper vertexShader;
	ShaderWrapper fragmentShader;
	GLuint p;
	ProgramWrapper(const char* vertexShaderCode, const char* fragmentShaderCode);

	virtual ~ProgramWrapper()noexcept{
		glDeleteProgram(this->p);
	}
};

class OpenGL2ShaderBase{
	ProgramWrapper program;
	
	const GLint matrixUniform;
	
	static const OpenGL2ShaderBase* boundShader;
public:
	OpenGL2ShaderBase(const char* vertexShaderCode, const char* fragmentShaderCode);
	
	OpenGL2ShaderBase(const OpenGL2ShaderBase&) = delete;
	OpenGL2ShaderBase& operator=(const OpenGL2ShaderBase&) = delete;
	
	virtual ~OpenGL2ShaderBase()noexcept{}

protected:
	GLint getUniform(const char* n);
	
	void bind()const{
		glUseProgram(program.p);
		assertOpenGLNoError();
		boundShader = this;
	}
	
	bool isBound()const noexcept{
		return this == boundShader;
	}
	
	void setUniformMatrix4f(GLint id, const r4::mat4f& m)const{
		glUniformMatrix4fv(id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&m));
		assertOpenGLNoError();
	}
	
	void setUniform4f(GLint id, float x, float y, float z, float a)const{
		glUniform4f(id, x, y, z, a);
		assertOpenGLNoError();
	}
	
	void setMatrix(const r4::mat4f& m)const{
		this->setUniformMatrix4f(this->matrixUniform, m);
	}
	
	static GLenum modeMap[];
	
	static GLenum modeToGLMode(morda::vertex_array::mode mode){
		return modeMap[unsigned(mode)];
	}
	
	void render(const r4::mat4f& m, const morda::vertex_array& va)const;
};

}
