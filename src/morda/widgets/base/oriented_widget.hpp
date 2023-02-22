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

#include "../widget.hpp"

namespace morda{

class oriented_widget : virtual public widget{
	bool is_vertical_v;

public:
	unsigned get_long_index()const noexcept{
		return this->is_vertical_v ? 1 : 0;
	}

	unsigned get_trans_index()const noexcept{
		return this->is_vertical_v ? 0 : 1;
	}

	oriented_widget(const utki::shared_ref<morda::context>& c, const treeml::forest& desc, bool vertical);

	oriented_widget(const oriented_widget&) = delete;
	oriented_widget& operator=(const oriented_widget&) = delete;

	bool is_vertical()const noexcept{
		return this->is_vertical_v;
	}

	void set_vertical(bool vertical);

private:

};

}
