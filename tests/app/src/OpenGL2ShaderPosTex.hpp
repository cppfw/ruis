#pragma once



#include "../../../src/morda/render/ShaderPosTex.hpp"

#include "OpenGL2Shader.hpp"


class OpenGL2ShaderPosTex : public morda::ShaderPosTex, public OpenGL2Shader{
	GLint texCoordAttrib;
	GLint textureUniform;
public:
	OpenGL2ShaderPosTex();
	
	void render(const kolme::Matr4f &m, const morda::Texture2D_n& tex, const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec2f> t, Mode_e mode = Mode_e::TRIANGLE_FAN)override;
	
	void render(const kolme::Matr4f& m, const morda::Texture2D_n& tex, const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec2f> t, Mode_e mode) override;
	
	void render(const kolme::Matr4f& m, const morda::Texture2D_n& tex, const morda::VertexBuffer& p, const morda::IndexBuffer& e, Mode_e mode) override;

	void render(const kolme::Matr4f& m, const morda::Texture2D_n& tex, const morda::VertexArray& va, const morda::IndexBuffer& e, Mode_e mode) override;

};
