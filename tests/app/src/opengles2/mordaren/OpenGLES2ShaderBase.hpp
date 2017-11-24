#pragma once

#include <utki/config.hpp>
#include <utki/debug.hpp>
#include <utki/Exc.hpp>
#include <utki/Buf.hpp>

#include <kolme/Matrix4.hpp>

#include <vector>

#include <morda/render/VertexArray.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

#include "OpenGLES2_util.hpp"

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



class OpenGLES2ShaderBase {
	ProgramWrapper program;
	
	const GLint matrixUniform;
	
	static const OpenGLES2ShaderBase* boundShader;
public:
	OpenGLES2ShaderBase(const char* vertexShaderCode, const char* fragmentShaderCode);
	
	OpenGLES2ShaderBase(const OpenGLES2ShaderBase&) = delete;
	OpenGLES2ShaderBase& operator=(const OpenGLES2ShaderBase&) = delete;
	
	virtual ~OpenGLES2ShaderBase()noexcept{}

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
	
	void setUniform4f(GLint id, float x, float y, float z, float a)const{
		glUniform4f(id, x, y, z, a);
		assertOpenGLNoError();
	}
	
	void setMatrix(const kolme::Matr4f& m)const{
		this->setUniformMatrix4f(this->matrixUniform, m);
		assertOpenGLNoError();
	}
	
	static GLenum modeMap[];
	
	static GLenum modeToGLMode(morda::VertexArray::Mode_e mode){
		return modeMap[unsigned(mode)];
	}
	
	void render(const kolme::Matr4f& m, const morda::VertexArray& va)const;
};

}
