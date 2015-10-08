#include "PosShader.hpp"


using namespace morda;


decltype(PosShader::quadFan) PosShader::quadFan = {{
	kolme::Vec2f(-1, -1), kolme::Vec2f(1, -1), kolme::Vec2f(1, 1), kolme::Vec2f(-1, 1)
}};


decltype(PosShader::quad01Fan) PosShader::quad01Fan = {{
	kolme::Vec2f(0, 0), kolme::Vec2f(1, 0), kolme::Vec2f(1, 1), kolme::Vec2f(0, 1)
}};
