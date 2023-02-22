/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "text.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace morda;

text::text(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
		widget(c, desc),
		single_line_text_widget(this->context, desc),
		color_widget(this->context, desc)
{}

void text::render(const morda::matrix4& matrix)const{
	morda::matrix4 matr(matrix);
	
	using std::round;
	
	const auto& font = this->get_font().get();

	matr.translate(
			-this->get_bounding_box().p.x(),
			round((font.get_height() + font.get_ascender() - font.get_descender()) / 2)
		);

	font.render(matr, morda::color_to_vec4f(this->get_current_color()), this->get_text());
}
