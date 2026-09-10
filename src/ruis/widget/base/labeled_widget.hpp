/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "../label/text.hpp"

#include "wrapped_widget.hpp"

namespace ruis {

class labeled_widget :
    virtual public widget, //
    private wrapped_widget<text>
{
public:
	struct parameters {
		color_widget::parameters color_params;
		text_widget::parameters text_params;
		ruis::string text;
	};

protected:
	labeled_widget(
		const utki::shared_ref<ruis::context>& context, //
		text& label
	) :
		widget(context, {}, {}),
		wrapped_widget<text>(
			context, //
			label
		)
	{}

public:
	text& get_label()
	{
		return this->get_bare();
	}
};

} // namespace ruis
