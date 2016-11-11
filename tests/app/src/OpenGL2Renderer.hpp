#pragma once

#include "../../../src/morda/render/Renderer.hpp"

#include "../../../src/morda/render/ShaderPosTex.hpp"

#include "OpenGL2Shader.hpp"


class OpenGL2ShaderPosTex : public morda::ShaderPosTex, public OpenGL2Shader{
	GLint texCoordAttrib;
	GLint textureUniform;
public:
	OpenGL2ShaderPosTex();
	
	void render(const kolme::Matr4f &m, const morda::Texture2D_n& tex, const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec2f> t, Mode_e mode = Mode_e::TRIANGLE_FAN)override;
	
	void render(const kolme::Matr4f& m, const morda::Texture2D_n& tex, const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec2f> t, Mode_e mode) override;
};


class OpenGL2Renderer : public morda::Renderer{
	OpenGL2ShaderPosTex sPosTex;
	
public:
	OpenGL2Renderer();
	
	OpenGL2Renderer(const OpenGL2Renderer&) = delete;
	OpenGL2Renderer& operator=(const OpenGL2Renderer&) = delete;
	
	virtual ~OpenGL2Renderer()noexcept;

	std::shared_ptr<morda::Texture2D_n> createTexture2D(TexType_e type, kolme::Vec2ui dim, const utki::Buf<std::uint8_t>& data) override;

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec3f> vertices) override;

	std::shared_ptr<morda::VertexArray> createVertexArray() override;

};
