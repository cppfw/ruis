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

#include "size_layout.hpp"

#include "../widget.hpp"

#include "../util/util.hpp"

using namespace morda;

const utki::shared_ref<size_layout> size_layout::instance = utki::make_shared<size_layout>();

void size_layout::lay_out(const vector2& size, semiconst_widget_list& widgets)const{
    for(const auto& w : widgets){
		if(w.get().is_layout_dirty()){
			auto d = dims_for_widget(w.get(), size);
			w.get().resize(d);
		}
	}
}
