#pragma once

#include "Texture2D_n.hpp"

#include "VertexBuffer.hpp"

#include <vector>

#include <kolme/Vector2.hpp>

#include <utki/Shared.hpp>
#include <utki/Buf.hpp>

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
	
	virtual std::shared_ptr<VertexBuffer> createVertexBuffer(const utki::Buf<kolme::Vec2f> vertices) = 0;
	
	std::shared_ptr<Renderer> getSharedPtr()noexcept{
		return this->sharedFromThis(this);
	}
};

}
