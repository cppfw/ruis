#include "SimpleSingleColoringShader.hpp"



SimpleSingleColoringShader::SimpleSingleColoringShader() :
		Shader(
				"#define highp\n #define mediump\n #define lowp\n"
				"attribute highp vec4 vertex;\n"
				"uniform lowp vec4 uniformColor;\n"
				"uniform highp mat4 matrix;\n"
				"varying lowp vec4 clr;\n"
				"void main(void){\n"
					"gl_Position = matrix * vertex;\n"
					"clr = uniformColor;\n"
				"}\n"
				,
				"#define highp\n #define mediump\n #define lowp\n"
				"varying lowp vec4 clr;\n"
				"void main(void){\n"
					"gl_FragColor = clr;\n"
				"}\n"
			)
{}
