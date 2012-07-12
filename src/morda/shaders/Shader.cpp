#include <ting/Array.hpp>

#include "Shader.hpp"


using namespace ting;
using namespace tride;



Shader::Shader(const char* vertexShaderCode, const char* fragmentShaderCode){
	this->vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(this->vertexShader, 1, &vertexShaderCode, 0);
	glCompileShader(this->vertexShader);
	if(this->CheckForCompileErrors(this->vertexShader)){
		TRACE(<< "Error while compiling:\n" << vertexShaderCode << std::endl)
		throw ting::Exc("Error compiling vertex shader");
	}

	this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragmentShader, 1, &fragmentShaderCode, 0);
	glCompileShader(this->fragmentShader);
	if(this->CheckForCompileErrors(this->fragmentShader)){
		TRACE(<< "Error while compiling:\n" << fragmentShaderCode << std::endl)
		throw ting::Exc("Error compiling fragment shader");
	}

	this->program = glCreateProgram();
	glAttachShader(this->program, this->vertexShader);
	glAttachShader(this->program, this->fragmentShader);
	glLinkProgram(this->program);
	if(this->CheckForLinkErrors(this->program)){
		TRACE(<< "Error while linking shader program" << std::endl)
		throw ting::Exc("Error linking shader program");
	}
}



Shader::~Shader()throw(){
	//make sure the shader objects are created before deleting them
	ASSERT(this->vertexShader != 0)
	ASSERT(this->fragmentShader != 0)
	ASSERT(this->program != 0)

	glDeleteProgram(this->program);
	glDeleteShader(this->fragmentShader);
	glDeleteShader(this->vertexShader);
}



bool Shader::CheckForCompileErrors(GLuint shader){
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
		}else{
			TRACE(<< "Shader::CheckForCompileErrors(): log length is 0" << std::endl)
		}
		return true;
	}
	return false;
}



bool Shader::CheckForLinkErrors(GLuint program){
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
		}else{
			TRACE(<< "Shader::CheckForLinkErrors(): log length is 0" << std::endl)
		}
		return true;
	}
	return false;
}



namespace{
static Vec2f quadTriangles[] = {
	Vec2f(-1, -1), Vec2f(-1, 1), Vec2f(1, 1), Vec2f(1, -1)
};
}//~namespace



void Shader::DrawQuad(GLenum mode){
	this->EnablePositionPointer();
	this->SetPositionPointer(quadTriangles);
	this->DrawArrays(mode, 4);
}



namespace{
static Vec2f quad01Triangles[] = {
	Vec2f(0, 0), Vec2f(0, 1), Vec2f(1, 1), Vec2f(1, 0)
};
}//~namespace



void Shader::DrawQuad01(GLenum mode){
	this->EnablePositionPointer();
	this->SetPositionPointer(quad01Triangles);
	this->DrawArrays(mode, 4);
}



static Vec2f quadTexCoords[] = {
	Vec2f(0, 0), Vec2f(0, 1), Vec2f(1, 1), Vec2f(1, 0)
};
