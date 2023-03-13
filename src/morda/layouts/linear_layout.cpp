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

#include "linear_layout.hpp"

using namespace morda;

const utki::shared_ref<row_layout> row_layout::instance = utki::make_shared<row_layout>();
const utki::shared_ref<column_layout> column_layout::instance = utki::make_shared<column_layout>();

linear_layout::linear_layout(bool is_vertical) :
    is_vertical(is_vertical)
{}

void linear_layout::lay_out(const vector2& size, semiconst_widget_list& widgets)const{
    // TODO:
}

vector2 linear_layout::measure(const vector2& quotum, const_widget_list& widgets)const{
    // TODO:
    return 0;
}
