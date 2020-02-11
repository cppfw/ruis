#include "Color.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

//TODO: remove
#include "../../gui.hpp"

using namespace morda;


Color::Color(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		ColorWidget(c, desc)
{}



void Color::render(const morda::Matr4r& matrix)const{
	applySimpleAlphaBlending();
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	auto& r = *morda::inst().context->renderer;
	r.shader->colorPos->render(matr, *r.posQuad01VAO, this->color());
}
