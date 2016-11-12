#pragma once

#include <vector>

#include <kolme/Vector2.hpp>

#include <utki/Shared.hpp>
#include <utki/Buf.hpp>

#include "Texture2D_n.hpp"

#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"

#include "ShaderPosTex.hpp"

namespace morda{

class Renderer : virtual public utki::Shared{
public:
	
	ShaderPosTex& shaderPosTex;
	
protected:
	Renderer(decltype(shaderPosTex) sPosTex) :
			shaderPosTex(sPosTex)
	{}
	
public:
	virtual ~Renderer()noexcept{}
	
	enum class TexType_e{
		GREY,
		GREYA,
		RGB,
		RGBA
	};
	
	static unsigned bytesPerPixel(TexType_e t);
	
	virtual std::shared_ptr<Texture2D_n> createTexture2D(TexType_e type, kolme::Vec2ui dim, const utki::Buf<std::uint8_t>& data) = 0;
	
	virtual std::shared_ptr<VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec3f> vertices) = 0;
	
	virtual std::shared_ptr<IndexBuffer> createIndexBuffer(const utki::Buf<std::uint16_t> indices) = 0;
	
	virtual std::shared_ptr<VertexArray> createVertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers) = 0;
	
	std::shared_ptr<Renderer> getSharedPtr()noexcept{
		return this->sharedFromThis(this);
	}
};

}
