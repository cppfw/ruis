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

#include "linear_container.hpp"

#include <cmath>

#include "../../util/util.hpp"
#include "../../context.hpp"

#include "../../layouts/linear_layout.hpp"

using namespace morda;

linear_container::linear_container(const utki::shared_ref<morda::context>& c, const treeml::forest& desc, bool vertical) :
		widget(c, desc),
		container(
			this->context,
			desc, 
			[&]() -> utki::shared_ref<morda::layout> {
				if(vertical){
					return column_layout::instance;
				}else{
					return row_layout::instance;
				}
			}()
		),
		oriented_widget(this->context, treeml::forest(), vertical)
{}

vector2 linear_container::measure(const vector2& quotum)const{
	return this->get_layout().measure(quotum, this->children());
}

void linear_container::lay_out(){
	this->get_layout().lay_out(this->rect().d, this->children());
}
