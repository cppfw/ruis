#pragma once

#include <morda/render/Factory.hpp>

#include "OpenGL2ShaderPosTex.hpp"


class render_factory : public morda::Factory{
public:
	render_factory();
	
	render_factory(const render_factory&) = delete;
	render_factory& operator=(const render_factory&) = delete;
	
	virtual ~render_factory()noexcept;

	std::shared_ptr<morda::Texture2D> createTexture2D(morda::Texture2D::TexType_e type, kolme::Vec2ui dim, utki::span<const std::uint8_t>& data) override;

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(utki::span<const kolme::Vec4f> vertices) override;
	
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(utki::span<const kolme::Vec3f> vertices) override;
	
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(utki::span<const kolme::Vec2f> vertices) override;

	std::shared_ptr<morda::IndexBuffer> createIndexBuffer(utki::span<const std::uint16_t> indices) override;
	
	std::shared_ptr<morda::VertexArray> createVertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, morda::VertexArray::Mode_e mode) override;

	std::unique_ptr<Shaders> createShaders() override;
	
	std::shared_ptr<morda::FrameBuffer> createFramebuffer(std::shared_ptr<morda::Texture2D> color) override;
};
