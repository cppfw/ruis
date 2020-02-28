#include "color.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

color::color(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		color_widget(this->context, desc)
{}

void color::render(const morda::matrix4& matrix)const{
	auto& r = *this->context->renderer;
	set_simple_alpha_blending(r);
	
	morda::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	r.shader->color_pos->render(matr, *r.pos_quad_01_vao, this->get_color());
}
