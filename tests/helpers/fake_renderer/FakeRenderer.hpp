#pragma once

#include "../../../src/morda/render/renderer.hpp"

class FakeFactory : public morda::RenderFactory{
public:
	std::shared_ptr<morda::FrameBuffer> createFramebuffer(std::shared_ptr<morda::Texture2D> color) override{
		return nullptr;
	}

	std::shared_ptr<morda::IndexBuffer> createIndexBuffer(const utki::span<std::uint16_t> indices) override{
		return nullptr;
	}

	std::unique_ptr<morda::RenderFactory::Shaders> createShaders() override{
		return nullptr;
	}

	std::shared_ptr<morda::Texture2D> createTexture2D(morda::Texture2D::TexType_e type, r4::vec2ui dim, const utki::span<std::uint8_t>& data) override{
		return nullptr;
	}

	std::shared_ptr<morda::VertexArray> createVertexArray(
			std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers,
			std::shared_ptr<morda::IndexBuffer> indices,
			morda::VertexArray::Mode_e mode
		) override
	{
		return nullptr;
	}

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::span<float> vertices) override{
		return nullptr;
	}

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::span<r4::vec2f> vertices) override{
		return nullptr;
	}
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::span<r4::vec3f> vertices) override{
		return nullptr;
	}

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::span<r4::vec4f> vertices) override{
		return nullptr;
	}

};

class FakeRenderer : public morda::renderer{
public:
	FakeRenderer() :
			morda::renderer(utki::makeUnique<FakeFactory>(), params())
	{}

	void clear_framebuffer() override{}
	r4::recti get_scissor() const override{
		return r4::recti(0);
	}
	r4::recti get_viewport() const override{
		return r4::recti(0);
	}
	bool is_scissor_enabled() const override{
		return false;
	}
	void set_blend_enabled(bool enable) override{}
	void set_blend_func(blend_factor src_color, blend_factor dst_color, blend_factor src_alpha, blend_factor dst_alpha) override{}
	void set_framebuffer_internal(morda::FrameBuffer* fb) override{}
	void set_scissor_enabled(bool enabled) override{}
	void set_scissor(r4::recti r) override{}
	void set_viewport(r4::recti r) override{}
};
