#pragma once

#include "../../../src/morda/render/Renderer.hpp"


class OpenGL2Renderer : public morda::Renderer{
public:
	OpenGL2Renderer();
	
	OpenGL2Renderer(const OpenGL2Renderer&) = delete;
	OpenGL2Renderer& operator=(const OpenGL2Renderer&) = delete;
	
	virtual ~OpenGL2Renderer()noexcept;

	std::shared_ptr<morda::Texture2D_n> createTexture2D(TexType_e type, unsigned width, const std::vector<std::uint8_t>& data) override;

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec2f> vertices) override;

};
