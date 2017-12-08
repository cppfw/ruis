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

	std::shared_ptr<morda::Texture2D> createTexture2D(morda::Texture2D::TexType_e type, kolme::Vec2ui dim, const utki::Buf<std::uint8_t>& data) override{
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

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec2f> vertices) override{
		return nullptr;
	}
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec3f> vertices) override{
		return nullptr;
	}

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec4f> vertices) override{
		return nullptr;
	}

};

class FakeRenderer : public morda::Renderer{
public:
	FakeRenderer() : morda::Renderer(utki::makeUnique<FakeFactory>(), Params()){}
	
	void clearFramebuffer() override{}
	kolme::Recti getScissorRect() const override{
		return kolme::Recti(0);
	}
	kolme::Recti getViewport() const override{
		return kolme::Recti(0);
	}
	bool isScissorEnabled() const override{
		return false;
	}
	void setBlendEnabled(bool enable) override{}
	void setBlendFunc(BlendFactor_e srcClr, BlendFactor_e dstClr, BlendFactor_e srcAlpha, BlendFactor_e dstAlpha) override{}
	void setFramebufferInternal(morda::FrameBuffer* fb) override{}
	void setScissorEnabled(bool enabled) override{}
	void setScissorRect(kolme::Recti r) override{}
	void setViewport(kolme::Recti r) override{}
};
