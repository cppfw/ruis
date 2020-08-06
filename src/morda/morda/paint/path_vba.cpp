#include "path_vba.hpp"

using namespace morda;

path_vba::path_vba(std::shared_ptr<morda::renderer> r, const path::vertices& path) :
		renderer(std::move(r))
{
	auto coreBuf = this->renderer->factory->create_vertex_buffer(utki::make_span(path.pos));
	
	this->core = this->renderer->factory->create_vertex_array(
			{{
				coreBuf,
			}},
			this->renderer->factory->create_index_buffer(utki::make_span(path.in_indices)),
			morda::vertex_array::mode::triangle_strip
		);
	
	
	this->border = this->renderer->factory->create_vertex_array(
			{{
				coreBuf,
				this->renderer->factory->create_vertex_buffer(utki::make_span(path.alpha)),
			}},
			this->renderer->factory->create_index_buffer(utki::make_span(path.out_indices)),
			morda::vertex_array::mode::triangle_strip
		);
}

void path_vba::render(const morda::matrix4& matrix, uint32_t color)const{
	if(!this->renderer || ! this->core){
		return;
	}

	this->renderer->shader->color_pos->render(matrix, *this->core, color);

	if(this->border){
		this->renderer->shader->color_pos_lum->render(matrix, *this->border, color);
	}
}
