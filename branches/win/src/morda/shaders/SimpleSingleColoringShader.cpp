#include "SimpleSingleColoringShader.hpp"



using namespace morda;



SimpleSingleColoringShader::SimpleSingleColoringShader() :
		Shader(
				"#ifndef GL_ES\n"
				"#define highp\n"
				"#define mediump\n"
				"#define lowp\n"
				"#endif\n"
				"attribute highp vec4 vertex;\n"
				"uniform lowp vec4 uniformColor;\n"
				"uniform highp mat4 matrix;\n"
				"varying lowp vec4 clr;\n"
				"void main(void){\n"
					"gl_Position = matrix * vertex;\n"
					"clr = uniformColor;\n"
				"}\n"
				,
				"#ifndef GL_ES\n"
				"#define highp\n"
				"#define mediump\n"
				"#define lowp\n"
				"#endif\n"
				"varying lowp vec4 clr;\n"
				"void main(void){\n"
					"gl_FragColor = clr;\n"
				"}\n"
			)
{}
