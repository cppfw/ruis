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

#include "oriented_widget.hpp"

using namespace morda;

oriented_widget::oriented_widget(const utki::shared_ref<morda::context>& c, const treeml::forest& desc, bool vertical) :
		widget(c, desc),
		is_vertical_v(vertical)
{}

void oriented_widget::set_vertical(bool vertical) {
	if(this->is_vertical_v == vertical){
		return;
	}
	this->is_vertical_v = vertical;
	this->invalidate_layout();
}
