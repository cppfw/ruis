#pragma once

#include <utki/Buf.hpp>
#include <utki/Unique.hpp>
#include <kolme/Vector2.hpp>

#include "Texture2D.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"

namespace morda{


class ShaderTexture : public utki::Unique{
public:
	ShaderTexture(){}
	
	ShaderTexture(const ShaderTexture&) = delete;
	ShaderTexture& operator=(const ShaderTexture&) = delete;
	
	virtual ~ShaderTexture()noexcept{}
	
	virtual void render(const kolme::Matr4f &m, const morda::VertexArray& va, const Texture2D& tex) = 0;
private:

};

}
