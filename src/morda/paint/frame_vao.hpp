#pragma once

#include "../render/renderer.hpp"

namespace morda{

class frame_vao{
	std::shared_ptr<morda::vertex_array> vao;
	
	std::shared_ptr<morda::renderer> renderer;
public:

	frame_vao(){}

	frame_vao(std::shared_ptr<morda::renderer> r, vector2 dims, vector2 thickness);

	void render(const matrix4& matrix, uint32_t color)const;
};

}
