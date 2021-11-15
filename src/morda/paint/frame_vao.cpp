#include "frame_vao.hpp"

using namespace morda;

frame_vao::frame_vao(std::shared_ptr<morda::renderer> r, vector2 dims, vector2 thickness) :
		renderer(std::move(r))
{
	std::array<vector2, 8> vertices = {{
		// outer
		{0, 0},
		{dims.x(), 0},
		dims,
		{0, dims.y()},

		// inner
		thickness,
		{dims.x() - thickness.x(), thickness.y()},
		{dims.x() - thickness.x(), dims.y() - thickness.y()},
		{thickness.x(), dims.y() - thickness.y()}
	}};

	std::array<uint16_t, 10> indices = {{
		4, 0, 1, 5, 2, 6, 3, 7, 4, 0
	}};

	auto vbo = this->renderer->factory->create_vertex_buffer(vertices);
	
	this->vao = this->renderer->factory->create_vertex_array(
			{
				vbo,
			},
			this->renderer->factory->create_index_buffer(indices),
			morda::vertex_array::mode::triangle_strip
		);
}

void frame_vao::render(const matrix4& matrix, uint32_t color)const{
	if(!this->renderer || ! this->vao){
		return;
	}

	this->renderer->shader->color_pos->render(matrix, *this->vao, color);
}
