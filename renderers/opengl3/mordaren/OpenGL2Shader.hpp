#pragma once

#include <GL/glew.h>

#include <utki/debug.hpp>
#include <utki/Exc.hpp>
#include <utki/Buf.hpp>

#include <kolme/Matrix4.hpp>

#include <vector>

#include "OpenGL2_util.hpp"

#include <morda/render/VertexArray.hpp>

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



class OpenGL2Shader {
	program_wrapper program;
	
	const GLint matrixUniform;
	
	static const OpenGL2Shader* boundShader;
public:
	OpenGL2Shader(const char* vertexShaderCode, const char* fragmentShaderCode);
	
	OpenGL2Shader(const OpenGL2Shader&) = delete;
	OpenGL2Shader& operator=(const OpenGL2Shader&) = delete;
	
	virtual ~OpenGL2Shader()noexcept{}

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
	
	void setUniformMatrix4f(GLint id, const kolme::Matr4f& m)const{
		glUniformMatrix4fv(id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&m));
		assertOpenGLNoError();
	}
	
	void setUniform4f(GLint id, float x, float y, float z, float a) {
		glUniform4f(id, x, y, z, a);
		assertOpenGLNoError();
	}
	
	void setMatrix(const kolme::Matr4f& m)const{
		this->setUniformMatrix4f(this->matrixUniform, m);
	}
	
	static GLenum modeMap[];
	
	static GLenum modeToGLMode(morda::VertexArray::Mode_e mode){
		return modeMap[unsigned(mode)];
	}
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va)const;
};

