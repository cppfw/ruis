#include "text.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace morda;

text::text(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		single_line_text_widget(this->context, desc),
		color_widget(this->context, desc)
{}

void text::render(const morda::matrix4& matrix)const{
	morda::matrix4 matr(matrix);
	
	using std::round;
	
	matr.translate(-this->get_bounding_box().p.x, round((this->get_font().get_height() + this->get_font().get_ascender() - this->get_font().get_descender()) / 2));

	this->get_font().render(matr, morda::color_to_vec4f(this->get_current_color()), this->get_text());
}
