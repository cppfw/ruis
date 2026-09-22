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

#include "refresh.hpp"

#include <string_view>

using namespace std::string_view_literals;

using namespace ruis;

refresh::refresh(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	spinning_image(
		context, //
		// clang-format off
		{
			.params = [&](){
				if(auto& src = params.params.spinning_image.image.specific.source; !src){
					src = context.get().loader().load<ruis::res::image>("ruis_img_refresh"sv);
				}
				if(auto& im = params.params.spinning_image.image.specific.disabled_img; !im){
					im = context.get().loader().load<ruis::res::image>("ruis_img_refresh_disabled"sv);
				}
				return std::move(params.params.spinning_image);
			}()
		} // clang-format on
	)
{}
