#pragma once

#include "Texture2D.hpp"
#include "VertexArray.hpp"

#include <utki/Unique.hpp>

#include <r4/matrix4.hpp>
#include <r4/vector4.hpp>

namespace morda{

class ShaderColorTexture{
public:
	ShaderColorTexture(){}
	
	ShaderColorTexture(const ShaderColorTexture&) = delete;
	ShaderColorTexture& operator=(const ShaderColorTexture&) = delete;
	
	virtual ~ShaderColorTexture()noexcept{}
	
	virtual void render(const r4::mat4f &m, const morda::VertexArray& va, r4::vec4f color, const morda::Texture2D& tex)const = 0;
};

}
