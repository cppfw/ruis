#pragma once

#include "ShaderPos.hpp"

#include <utki/Buf.hpp>
#include <kolme/Vector2.hpp>

#include "Texture2D_n.hpp"

namespace morda{


class ShaderPosTex : public ShaderPos{
public:
	ShaderPosTex(){}
	
	ShaderPosTex(const ShaderPosTex&) = delete;
	ShaderPosTex& operator=(const ShaderPosTex&) = delete;
	
	virtual void render(const kolme::Matr4f &m, const Texture2D_n& tex, const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec2f> t, Mode_e mode = Mode_e::TRIANGLE_FAN) = 0;
	
	virtual void render(const kolme::Matr4f &m, const Texture2D_n& tex, const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec2f> t, Mode_e mode = Mode_e::TRIANGLE_FAN) = 0;
private:

};

}
