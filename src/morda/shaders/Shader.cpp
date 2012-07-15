#include <ting/Array.hpp>

#include "Shader.hpp"

#include"../Application.hpp"



using namespace morda;



Shader::ShaderWrapper::ShaderWrapper(const char* code, GLenum type){
	if(!morda::Application::IsCreated()){
		throw morda::Exc("Application singleton object is not created, OpenGL is not initialized, cannot create shader.");
	}
	
	this->s = glCreateShader(type);
	
	glShaderSource(this->s, 1, &code, 0);
	glCompileShader(this->s);
	if(this->CheckForCompileErrors(this->s)){
		TRACE(<< "Error while compiling:\n" << code << std::endl)
		glDeleteShader(this->s);
		throw ting::Exc("Error compiling vertex shader");
	}
}



Shader::ProgramWrapper::ProgramWrapper(GLuint vertex, GLuint fragment){
	this->p = glCreateProgram();
	glAttachShader(this->p, vertex);
	glAttachShader(this->p, fragment);
	glLinkProgram(this->p);
	if(this->CheckForLinkErrors(this->p)){
		TRACE(<< "Error while linking shader program" << std::endl)
		glDeleteProgram(this->p);
		throw ting::Exc("Error linking shader program");
	}
}



Shader::Shader(const char* vertexShaderCode, const char* fragmentShaderCode) :
		vertexShader(vertexShaderCode, GL_VERTEX_SHADER),
		fragmentShader(fragmentShaderCode, GL_FRAGMENT_SHADER),
		program(this->vertexShader.s, this->fragmentShader.s)
{
	this->positionAttr = this->GetAttribute("vertex");
	this->matrixUniform = this->GetUniform("matrix");
}



bool Shader::ShaderWrapper::CheckForCompileErrors(GLuint shader){
	GLint value = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
	if(value == 0){ //if not compiled
		GLint logLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		if(logLen > 1){//1 char is a terminating 0
			ting::Array<char> log(logLen);
			GLint len;
			glGetShaderInfoLog(shader, log.Size(), &len, &log[0]);
			TRACE(<< "===Compile log===\n" << log.Begin() << std::endl)
		}
		return true;
	}
	return false;
}



bool Shader::ProgramWrapper::CheckForLinkErrors(GLuint program){
	GLint value = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &value);
	if(value == 0){ //if not linked
		GLint logLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		if(logLen > 1){ //1 is for terminating 0 character.
			ting::Array<char> log(logLen);
			GLint len;
			glGetProgramInfoLog(program, log.Size(), &len, &log[0]);
			TRACE(<< "===Link log===\n" << log.Begin() << std::endl)
		}
		return true;
	}
	return false;
}



namespace{
static tride::Vec2f quadTriangles[] = {
	tride::Vec2f(-1, -1), tride::Vec2f(-1, 1), tride::Vec2f(1, 1), tride::Vec2f(1, -1)
};
}//~namespace



void Shader::DrawQuad(GLenum mode){
	this->EnablePositionPointer();
	this->SetPositionPointer(quadTriangles);
	this->DrawArrays(mode, 4);
}



namespace{
static tride::Vec2f quad01Triangles[] = {
	tride::Vec2f(0, 0), tride::Vec2f(0, 1), tride::Vec2f(1, 1), tride::Vec2f(1, 0)
};
}//~namespace



void Shader::DrawQuad01(GLenum mode){
	this->EnablePositionPointer();
	this->SetPositionPointer(quad01Triangles);
	this->DrawArrays(mode, 4);
}
