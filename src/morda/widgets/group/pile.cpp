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

#include "pile.hpp"

#include "../../util/util.hpp"

#include "../../layouts/pile_layout.hpp"

using namespace morda;

pile::pile(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
		widget(c, desc),
		container(this->context, desc, pile_layout::instance)
{}

vector2 pile::measure(const vector2& quotum)const{
	return this->get_layout().measure(quotum, this->children());
}

void pile::lay_out(){
	this->get_layout().lay_out(this->rect().d, this->children());
}
