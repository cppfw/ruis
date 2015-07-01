#include "Shader.hpp"



using namespace morda;


Shader* Shader::boundShader = nullptr;





Shader::Shader(const char* vertexShaderCode, const char* fragmentShaderCode) :
		program(Render::compileShader(vertexShaderCode, fragmentShaderCode)),
		matrixUniform(this->getUniform("matrix"))
{}
