#pragma once

#include <morda/render/RenderFactory.hpp>


namespace mordaren{

class OpenGLES2Factory : public morda::RenderFactory{
public:
	OpenGLES2Factory();
	
	OpenGLES2Factory(const OpenGLES2Factory&) = delete;
	OpenGLES2Factory& operator=(const OpenGLES2Factory&) = delete;
	
	virtual ~OpenGLES2Factory()noexcept;

	std::shared_ptr<morda::Texture2D> createTexture2D(morda::Texture2D::TexType_e type, kolme::Vec2ui dim, const utki::Buf<std::uint8_t>& data) override;

	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec4f> vertices) override;
	
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec3f> vertices) override;
	
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec2f> vertices) override;
	
	std::shared_ptr<morda::VertexBuffer> createVertexBuffer(const utki::Buf<float> vertices) override;

	std::shared_ptr<morda::IndexBuffer> createIndexBuffer(const utki::Buf<std::uint16_t> indices) override;
	
	std::shared_ptr<morda::VertexArray> createVertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, morda::VertexArray::Mode_e mode) override;

	std::unique_ptr<Shaders> createShaders() override;
	
	std::shared_ptr<morda::FrameBuffer> createFramebuffer(std::shared_ptr<morda::Texture2D> color) override;
};

}
