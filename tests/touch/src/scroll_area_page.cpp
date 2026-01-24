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

#include "scroll_area_page.hpp"

#include <ruis/widget/button/impl/rectangle_push_button.hpp>
#include <ruis/widget/group/touch/scroll_area.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/widget/slider/slider.hpp>

#include "style.hpp"

using namespace std::string_literals;

using namespace ruis::length_literals;

namespace {

ruis::widget_list make_scroll_area_page_contents(utki::shared_ref<ruis::context> c)
{
	// clang-format off
    return {
        m::push_button(c,
            {
                .layout_params{
                    .dims{ruis::dim::fill, ruis::dim::min}
                }
            },
            {
                m::text(c, {}, U"Button 1"s)
            }
        ),
        m::slider(c,
            {
                .layout_params{
                    .dims{50_pp, 300_pp}
                },
                .fraction_params{
                    .fraction = 0.5f
                },
                .oriented_params{
                    .vertical = true
                }
            }
        ),
    };
    //clang-format on
}

class scroll_area_page :
	public ruis::page, //
	private ruis::touch::scroll_area
{
public:
	scroll_area_page(utki::shared_ref<ruis::context> c) :
		ruis::widget(std::move(c), {}, {}),
		ruis::page(this->context, {}),
		// clang-format off
        ruis::touch::scroll_area(
            this->context, //
            {
                .layout_params = { // TODO: is needed?
                    .dims = {ruis::dim::fill, ruis::dim::fill}
                }
            },
            make_scroll_area_page_contents(this->context)
        )
	// clang-format on
	{}
};

} // namespace

utki::shared_ref<ruis::page> make_scroll_area_page(utki::shared_ref<ruis::context> c)
{
	return utki::make_shared<scroll_area_page>(std::move(c));
}
