#include "SimpleSingleColoringShader.hpp"



SimpleSingleColoringShader::SimpleSingleColoringShader() :
		Shader(
				"attribute " /*highp*/" vec4 vertex;\n"
				"uniform " /*lowp*/" vec4 vertexColor;\n"
				"uniform " /*highp*/" mat4 matrix;\n"
				"varying "/*lowp*/" vec4 color;\n"
				"void main(void){\n"
					"gl_Position = matrix * vertex;\n"
					"color = vertexColor;\n"
				"}\n",

				"varying "/*lowp*/" vec4 color;\n"
				"void main(void){\n"
					"gl_FragColor = color;\n"
				"}\n"
			)
{
	this->positionAttr = glGetAttribLocation(this->program, "vertex");
	
	this->colorUniform = glGetUniformLocation(this->program, "vertexColor");
	this->matrixUniform = glGetUniformLocation(this->program, "matrix");
}
