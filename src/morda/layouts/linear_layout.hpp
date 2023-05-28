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

#pragma once

#include "../layout.hpp"
#include "../util/oriented.hpp"

namespace morda {

// TODO: consider inheriting oriented
class linear_layout : public layout, public oriented
{
public:
	linear_layout(bool is_vertical);

	void lay_out(const vector2& size, semiconst_widget_list& widgets) const override;

	vector2 measure(const vector2& quotum, const_widget_list& widgets) const override;
};

class row_layout : public linear_layout
{
public:
	row_layout() :
		linear_layout(false)
	{}

	static const utki::shared_ref<row_layout> instance;
};

class column_layout : public linear_layout
{
public:
	column_layout() :
		linear_layout(true)
	{}

	static const utki::shared_ref<column_layout> instance;
};

} // namespace morda
