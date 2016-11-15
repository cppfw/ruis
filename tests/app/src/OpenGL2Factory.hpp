#pragma once

#include "../../../src/morda/render/Factory.hpp"

#include "OpenGL2ShaderPosTex.hpp"


class OpenGL2Factory : public morda::Factory{
public:
	OpenGL2Factory();
	
	OpenGL2Factory(const OpenGL2Factory&) = delete;
	OpenGL2Factory& operator=(const OpenGL2Factory&) = delete;
	
	virtual ~OpenGL2Factory()noexcept;

	std::shared_ptr<morda::Texture2D_n> createTexture2D(TexType_e type, kolme::Vec2ui dim, const utki::Buf<std::uint8_t>& data) override;

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec3f> vertices) override;
	
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec2f> vertices) override;

	std::shared_ptr<morda::IndexBuffer> createIndexBuffer(const utki::Buf<std::uint16_t> indices) override;
	
	std::shared_ptr<morda::VertexArray> createVertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, morda::VertexArray::Mode_e mode) override;

	std::unique_ptr<morda::ShaderPosTex> createPosTexShader() override;
	
	std::unique_ptr<morda::ShaderColorPos> createColorPosShader() override;

};
