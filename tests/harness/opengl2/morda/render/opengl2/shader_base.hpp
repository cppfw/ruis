#pragma once

#include <utki/config.hpp>
#include <utki/debug.hpp>

#include <r4/matrix4.hpp>

#include <vector>

#include "util.hpp"

#include <morda/render/vertex_array.hpp>

#include <GL/glew.h>

namespace morda{ namespace render_opengl2{

struct shader_wrapper{
	GLuint s;
	shader_wrapper(const char* code, GLenum type);
	~shader_wrapper()noexcept{
		glDeleteShader(this->s);
	}
};

struct program_wrapper{
	shader_wrapper vertexShader;
	shader_wrapper fragmentShader;
	GLuint p;
	program_wrapper(const char* vertexShaderCode, const char* fragmentShaderCode);

	virtual ~program_wrapper()noexcept{
		glDeleteProgram(this->p);
	}
};

class OpenGL2ShaderBase{
	program_wrapper program;
	
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
	
	void setUniformMatrix4f(GLint id, const r4::matrix4<float>& m)const{
		glUniformMatrix4fv(id, 1, GL_TRUE, reinterpret_cast<const GLfloat*>(&m));
		assertOpenGLNoError();
	}
	
	void setUniform4f(GLint id, float x, float y, float z, float a)const{
		glUniform4f(id, x, y, z, a);
		assertOpenGLNoError();
	}
	
	void setMatrix(const r4::matrix4<float>& m)const{
		this->setUniformMatrix4f(this->matrixUniform, m);
	}
	
	static GLenum modeMap[];
	
	static GLenum modeToGLMode(morda::vertex_array::mode mode){
		return modeMap[unsigned(mode)];
	}
	
	void render(const r4::matrix4<float>& m, const morda::vertex_array& va)const;
};

}}
