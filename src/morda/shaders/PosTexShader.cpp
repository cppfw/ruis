#include "PosTexShader.hpp"



using namespace morda;



decltype(PosTexShader::quadFanTexCoords) PosTexShader::quadFanTexCoords = {{
	Vec2f(0, 0), Vec2f(1, 0), Vec2f(1, 1), Vec2f(0, 1)
}};



PosTexShader::PosTexShader() :
		Shader(
				R"qwertyuiop(
						UNIFORM MAT4F matrix;

						ATTRIB_BEGIN
							ATTRIB VEC4F pos;
							ATTRIB VEC2F texCoord0;
						ATTRIB_END

						VARYING_BEGIN
							VARYING VEC2F tc0;
						VARYING_END

						VERTEX_MAIN_BEGIN
							gl_Position = matrix * pos;
							tc0 = texCoord0;
						VERTEX_MAIN_END
					)qwertyuiop",
				R"qwertyuiop(
						UNIFORM sampler2D textureNumber;
		
						VARYING VEC2F tc0;
		
						FRAG_MAIN_BEGIN
							gl_FragColor = texture2D(textureNumber, tc0);
						FRAG_MAIN_END
					)qwertyuiop"
			),
		texCoordAttr(this->getAttribute("texCoord0")),
		texNumberUniform(this->getUniform("textureNumber"))
{}
