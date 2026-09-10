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

#include "text_widget.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

text_widget::text_widget(
	const utki::shared_ref<ruis::context>& context, //
	parameters params
) :
	color_widget(
		context,
		[&]() {
			if (params.color_params.color.get().is_undefined()) {
				params.color_params.color = context.get().style().get_color_text();
			}
			if (params.color_params.disabled_color.get().is_undefined()) {
				params.color_params.disabled_color = context.get().style().get_color_text_secondary();
			}
			return std::move(params.color_params);
		}()
	),
	font_widget(context, std::move(params.font_params)),
	params(std::move(params))
{}
