#pragma once

#include <vector>

#include <r4/vector2.hpp>

#include <utki/shared.hpp>
#include <utki/Unique.hpp>
#include <utki/Buf.hpp>

#include "Texture2D.hpp"

#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"

#include "ShaderTexture.hpp"
#include "ShaderColor.hpp"
#include "Shader.hpp"
#include "ShaderColorTexture.hpp"
#include "FrameBuffer.hpp"

namespace morda{

class RenderFactory : public utki::Unique{
protected:
	RenderFactory(){}
	
public:
	virtual ~RenderFactory()noexcept{}	
	
	virtual std::shared_ptr<Texture2D> createTexture2D(Texture2D::TexType_e type, r4::vec2ui dim, const utki::span<std::uint8_t>& data) = 0;
	
	std::shared_ptr<Texture2D> createTexture2D(r4::vec2ui dim, const utki::span<std::uint32_t>& data);
	
	virtual std::shared_ptr<VertexBuffer> createVertexBuffer(const utki::span<r4::vec4f> vertices) = 0;
	
	virtual std::shared_ptr<VertexBuffer> createVertexBuffer(const utki::span<r4::vec3f> vertices) = 0;
	
	virtual std::shared_ptr<VertexBuffer> createVertexBuffer(const utki::span<r4::vec2f> vertices) = 0;
	
	virtual std::shared_ptr<VertexBuffer> createVertexBuffer(const utki::span<float> vertices) = 0;
	
	virtual std::shared_ptr<IndexBuffer> createIndexBuffer(const utki::span<std::uint16_t> indices) = 0;
	
	virtual std::shared_ptr<VertexArray> createVertexArray(
			std::vector< std::shared_ptr<morda::VertexBuffer> >&& buffers,
			std::shared_ptr<morda::IndexBuffer> indices,
			VertexArray::Mode_e mode
		) = 0;
	
	struct Shaders : public utki::Unique{
		std::unique_ptr<ShaderTexture> posTex;
		std::unique_ptr<ShaderColor> colorPos;
		std::unique_ptr<ShaderColor> colorPosLum;
		std::unique_ptr<Shader> posClr;
		std::unique_ptr<ShaderColorTexture> colorPosTex;
	};
	
	virtual std::unique_ptr<Shaders> createShaders() = 0;
	
	virtual std::shared_ptr<FrameBuffer> createFramebuffer(std::shared_ptr<Texture2D> color) = 0;
};

}
