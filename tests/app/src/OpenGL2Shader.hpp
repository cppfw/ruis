#pragma once

#include <GL/glew.h>

#include <utki/debug.hpp>
#include <utki/Exc.hpp>
#include <utki/Buf.hpp>

#include <kolme/Matrix4.hpp>

#include <vector>

#include "../../../src/morda/render/Shader_n.hpp"

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



class OpenGL2Shader {
	ProgramWrapper program;
	
	const GLint posAttrib;
	
	const GLint matrixUniform;
public:
	OpenGL2Shader(const char* vertexShaderCode, const char* fragmentShaderCode);
	
	OpenGL2Shader(const OpenGL2Shader&) = delete;
	OpenGL2Shader& operator=(const OpenGL2Shader&) = delete;
	
	virtual ~OpenGL2Shader()noexcept{}

protected:
	GLint getUniform(const char* n);
	
	GLint getAttribute(const char* n);
	
	void bind(){
		glUseProgram(program.p);
//		AssertOpenGLNoError();
	}
	
	void setUniformMatrix4f(GLint id, const kolme::Matr4f& m){
		glUniformMatrix4fv(id, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&m));
//		AssertOpenGLNoError();
	}
	
	void setMatrix(const kolme::Matr4f& m){
		this->setUniformMatrix4f(this->matrixUniform, m);
	}
	
	void setPosAttribArray(const kolme::Vec3f* a);
	
	void setVertexAttribArray(GLint id, const kolme::Vec3f* a);
	
	void setVertexAttribArray(GLint id, const kolme::Vec2f* a);
	
	void renderElements(morda::Shader_n::Mode_e mode, const utki::Buf<std::uint16_t>& i);
};

