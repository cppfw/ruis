#pragma once

#include "../../src/morda/render/Renderer.hpp"

class FakeFactory : public morda::RenderFactory{
public:
	std::shared_ptr<morda::FrameBuffer> createFramebuffer(std::shared_ptr<morda::Texture2D> color) override{
		return nullptr;
	}
	
	std::shared_ptr<morda::IndexBuffer> createIndexBuffer(const utki::Buf<std::uint16_t> indices) override{
		return nullptr;
	}
	
	std::unique_ptr<morda::RenderFactory::Shaders> createShaders() override{
		return nullptr;
	}

	std::shared_ptr<morda::Texture2D> createTexture2D(morda::Texture2D::TexType_e type, r4::vec2ui dim, const utki::Buf<std::uint8_t>& data) override{
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
	
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<float> vertices) override{
		return nullptr;
	}

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<r4::vec2f> vertices) override{
		return nullptr;
	}
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<r4::vec3f> vertices) override{
		return nullptr;
	}

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<r4::vec4f> vertices) override{
		return nullptr;
	}

};

class FakeRenderer : public morda::Renderer{
public:
	FakeRenderer() :
			morda::Renderer(utki::makeUnique<FakeFactory>(), Params())
	{}
	
	void clearFramebuffer() override{}
	r4::recti getScissorRect() const override{
		return r4::recti(0);
	}
	r4::recti getViewport() const override{
		return r4::recti(0);
	}
	bool isScissorEnabled() const override{
		return false;
	}
	void setBlendEnabled(bool enable) override{}
	void setBlendFunc(BlendFactor_e srcClr, BlendFactor_e dstClr, BlendFactor_e srcAlpha, BlendFactor_e dstAlpha) override{}
	void setFramebufferInternal(morda::FrameBuffer* fb) override{}
	void setScissorEnabled(bool enabled) override{}
	void setScissorRect(r4::recti r) override{}
	void setViewport(r4::recti r) override{}
};
