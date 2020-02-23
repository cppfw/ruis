#include "Color.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

//TODO: remove
#include "../../gui.hpp"

using namespace morda;


Color::Color(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		ColorWidget(this->context, desc)
{}



void Color::render(const morda::Matr4r& matrix)const{
	auto& r = *this->context->renderer;
	applySimpleAlphaBlending(r);
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	r.shader->colorPos->render(matr, *r.pos_quad_01_vao, this->color());
}
