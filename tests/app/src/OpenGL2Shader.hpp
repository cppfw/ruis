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
	ShaderWrapper(const char* code, GLenum type){
		this->s = glCreateShader(type);
	
		if(this->s == 0){
			throw utki::Exc("glCreateShader() failed");
		}

		const char* c = code;

		glShaderSource(this->s, 1, &c, 0);
		glCompileShader(this->s);
		if(this->CheckForCompileErrors(this->s)){
			TRACE(<< "Error while compiling:\n" << c << std::endl)
			glDeleteShader(this->s);
			throw utki::Exc("Error compiling shader");
		}
	}
	~ShaderWrapper()noexcept{
		glDeleteShader(this->s);
	}

	//return true if not compiled
	static bool CheckForCompileErrors(GLuint shader){
		GLint value = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
		if(value == 0){ //if not compiled
			GLint logLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
			if(logLen > 1){//1 char is a terminating 0
				std::vector<char> log(logLen);
				GLint len;
				glGetShaderInfoLog(shader, GLsizei(log.size()), &len, &*log.begin());
				TRACE(<< "===Compile log===\n" << &*log.begin() << std::endl)
			}else{
				TRACE(<< "Shader compile log is empty" << std::endl)
			}
			return true;
		}
		return false;
	}
};



struct ProgramWrapper{
	ShaderWrapper vertexShader;
	ShaderWrapper fragmentShader;
	GLuint p;
	ProgramWrapper(const char* vertexShaderCode, const char* fragmentShaderCode) :
			vertexShader(vertexShaderCode, GL_VERTEX_SHADER),
			fragmentShader(fragmentShaderCode, GL_FRAGMENT_SHADER)
	{
		this->p = glCreateProgram();
		glAttachShader(this->p, vertexShader.s);
		glAttachShader(this->p, fragmentShader.s);
		glLinkProgram(this->p);
		if(this->CheckForLinkErrors(this->p)){
			TRACE(<< "Error while linking shader program" << vertexShaderCode << std::endl << fragmentShaderCode << std::endl)
			glDeleteProgram(this->p);
			throw utki::Exc("Error linking shader program");
		}
	}

	virtual ~ProgramWrapper()noexcept{
		glDeleteProgram(this->p);
	}

	//return true if not linked
	static bool CheckForLinkErrors(GLuint program){
		GLint value = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &value);
		if(value == 0){ //if not linked
			GLint logLen = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
			if(logLen > 1){ //1 is for terminating 0 character.
				std::vector<char> log(logLen);
				GLint len;
				glGetProgramInfoLog(program, GLsizei(log.size()), &len, &*log.begin());
				TRACE(<< "===Link log===\n" << &*log.begin() << std::endl)
			}
			return true;
		}
		return false;
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

