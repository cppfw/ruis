#pragma once

#include "../../../src/morda/render/Renderer.hpp"

#include "OpenGL2ShaderPosTex.hpp"


class OpenGL2Renderer : public morda::Renderer{
	OpenGL2ShaderPosTex sPosTex;
	
public:
	OpenGL2Renderer();
	
	OpenGL2Renderer(const OpenGL2Renderer&) = delete;
	OpenGL2Renderer& operator=(const OpenGL2Renderer&) = delete;
	
	virtual ~OpenGL2Renderer()noexcept;

	std::shared_ptr<morda::Texture2D_n> createTexture2D(TexType_e type, kolme::Vec2ui dim, const utki::Buf<std::uint8_t>& data) override;

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec3f> vertices) override;
	
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec2f> vertices) override;

	std::shared_ptr<morda::IndexBuffer> createIndexBuffer(const utki::Buf<std::uint16_t> indices) override;
	
	std::shared_ptr<morda::VertexArray> createVertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices) override;

};
