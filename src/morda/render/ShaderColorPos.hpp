#pragma once

#include <kolme/Matrix4.hpp>

#include "Shader.hpp"
#include "VertexArray.hpp"

namespace morda{

class ShaderColorPos : public Shader_n{
public:
	ShaderColorPos(){}
	
	ShaderColorPos(const ShaderColorPos&) = delete;
	ShaderColorPos& operator=(const ShaderColorPos&) = delete;
	
	virtual void render(const kolme::Matr4f &m, kolme::Vec4f color, const VertexArray& va) = 0;

	void render(const kolme::Matr4f &m, std::uint32_t color, const VertexArray& va){
		this->render(
				m,
				kolme::Vec4f(
						float(color & 0xff) / float(0xff),
						float((color >> 8) & 0xff) / float(0xff),
						float((color >> 16) & 0xff) / float(0xff),
						float((color >> 24) & 0xff) / float(0xff)
					),
				va
			);
	}
};

}
