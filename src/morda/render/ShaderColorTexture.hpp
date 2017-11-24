#pragma once

#include "Texture2D.hpp"
#include "VertexArray.hpp"

#include <utki/Unique.hpp>

#include <kolme/Matrix4.hpp>
#include <kolme/Vector4.hpp>

namespace morda{

class ShaderColorTexture : public utki::Unique{
public:
	ShaderColorTexture(){}
	
	ShaderColorTexture(const ShaderColorTexture&) = delete;
	ShaderColorTexture& operator=(const ShaderColorTexture&) = delete;
	
	virtual ~ShaderColorTexture()noexcept{}
	
	virtual void render(const kolme::Matr4f &m, const morda::VertexArray& va, kolme::Vec4f color, const morda::Texture2D& tex)const = 0;
};

}
