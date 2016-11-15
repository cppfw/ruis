#pragma once

#include "Shader_n.hpp"

#include <utki/Buf.hpp>
#include <kolme/Vector2.hpp>

#include "Texture2D_n.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"

namespace morda{


class ShaderPosTex : public Shader_n{
public:
	ShaderPosTex(){}
	
	ShaderPosTex(const ShaderPosTex&) = delete;
	ShaderPosTex& operator=(const ShaderPosTex&) = delete;
	
	virtual void render(const kolme::Matr4f &m, const Texture2D_n& tex, const morda::VertexArray& va) = 0;
private:

};

}
