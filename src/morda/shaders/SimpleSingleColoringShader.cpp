#include "SimpleSingleColoringShader.hpp"



SimpleSingleColoringShader::SimpleSingleColoringShader(){
	this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char *vsrc[1] = {
		"attribute " /*highp*/" vec4 vertex;\n"
		"uniform " /*lowp*/" vec4 vertexColor;\n"
		"uniform " /*highp*/" mat4 matrix;\n"
		"varying "/*lowp*/" vec4 color;\n"
		"void main(void){\n"
			"gl_Position = matrix * vertex;\n"
			"color = vertexColor;\n"
		"}\n"
	};

	glShaderSource(this->vertexShader, 1, vsrc, 0);
	glCompileShader(this->vertexShader);
	if(this->CheckForCompileErrors(this->vertexShader)){
		TRACE(<< "Error while compiling:\n" << vsrc[0] << std::endl)
		throw ting::Exc("Error compiling vertex shader");
	}

	this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fsrc[1] = {
		"varying "/*lowp*/" vec4 color;\n"
		"void main(void){\n"
			"gl_FragColor = color;\n"
		"}\n"
	};
	glShaderSource(this->fragmentShader, 1, fsrc, 0);
	glCompileShader(this->fragmentShader);
	if(this->CheckForCompileErrors(this->fragmentShader)){
		TRACE(<< "Error while compiling:\n" << fsrc[0] << std::endl)
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

	this->positionAttr = glGetAttribLocation(this->program, "vertex");
	
	this->colorUniform = glGetUniformLocation(this->program, "vertexColor");
	this->matrixUniform = glGetUniformLocation(this->program, "matrix");
}
