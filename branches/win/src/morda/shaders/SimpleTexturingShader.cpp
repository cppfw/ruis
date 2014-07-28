#include "SimpleTexturingShader.hpp"



using namespace morda;



SimpleTexturingShader::SimpleTexturingShader() :
		Shader(
				"#ifndef GL_ES\n"
				"#define highp\n"
				"#define mediump\n"
				"#define lowp\n"
				"#endif\n"
				"attribute highp vec4 vertex;\n"
				"attribute highp vec2 textureCoord;\n"
				"uniform highp mat4 matrix;\n"
				"varying highp vec2 texCoord;\n"
				"void main(void){\n"
					"gl_Position = matrix * vertex;\n"
					"texCoord = textureCoord;\n"
				"}\n"
				,
				"#ifndef GL_ES\n"
				"#define highp\n"
				"#define mediump\n"
				"#define lowp\n"
				"#endif\n"
				"uniform sampler2D textureNumber;\n"
				"varying highp vec2 texCoord;\n"
				"void main(void){\n"
					"gl_FragColor = texture2D(textureNumber, texCoord);\n"
				"}\n"
			)
{}
