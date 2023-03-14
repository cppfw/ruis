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

#include "layout.hpp"

using namespace morda;

vector2 layout::measure(const vector2& quotum, const_widget_list& widgets)const{
    return max(quotum, 0);
}

void layout::lay_out(const vector2& size, semiconst_widget_list& widgets)const{
    for(auto& w : widgets){
		if(w.get().is_layout_dirty()){
			w.get().layout_dirty = false;
			w.get().lay_out();
		}
	}
}
