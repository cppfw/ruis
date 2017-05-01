#pragma once

#include "Shader.hpp"

#include <utki/Buf.hpp>
#include <kolme/Vector2.hpp>

#include "Texture2D.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"

namespace morda{


class ShaderPosTex : public Shader{
public:
	ShaderPosTex(){}
	
	ShaderPosTex(const ShaderPosTex&) = delete;
	ShaderPosTex& operator=(const ShaderPosTex&) = delete;
	
	virtual void render(const kolme::Matr4f &m, const Texture2D& tex, const morda::VertexArray& va) = 0;
private:

};

}
