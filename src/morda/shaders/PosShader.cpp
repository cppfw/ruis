#include "PosShader.hpp"


using namespace morda;


decltype(PosShader::quadFan) PosShader::quadFan = {{
	Vec2f(-1, -1), Vec2f(1, -1), Vec2f(1, 1), Vec2f(-1, 1)
}};


decltype(PosShader::quad01Fan) PosShader::quad01Fan = {{
	Vec2f(0, 0), Vec2f(1, 0), Vec2f(1, 1), Vec2f(0, 1)
}};
