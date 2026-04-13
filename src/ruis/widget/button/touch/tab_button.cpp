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

#include "tab_button.hpp"

#include "../../label/padding.hpp"

using namespace ruis::touch;

using namespace ruis::length_literals;

namespace {
constexpr auto button_padding = 10_pp;
} // namespace

tab_button::tab_button(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	ruis::string text
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	button(
		this->context,
		{
}
	),
	toggle_button(this->context),
	choice_button(this->context),
	// clang-format off
    ruis::container(
        this->context, //
        {
            .container_params = {
                .layout = ruis::layout::pile
            }
        },
        {
            ruis::make::padding(
                this->context,
                {
                    .layout_params = {
                        .dims = {ruis::dim::fill, ruis::dim::fill}
                    },
                    .container_params = {
                        .layout = ruis::layout::column
                    },
                    .padding_params = {
                        .borders = {button_padding}
                    }
                },
                {
                    ruis::make::image(this->context,
                        {
                            .layout_params = {
                                .dims = {ruis::dim::min, ruis::dim::fill},
                                .weight = 1
                            },
                            .image_params = std::move(params.image_params)
                        }
                    ),
                    ruis::make::text(this->context,
                        {
                            .layout_params = {
                                .dims = {ruis::dim::min, ruis::dim::min},
                                .align = {ruis::align::center, ruis::align::center}
                            }
                        },
                        std::move(text)
                    )
                }
            )
        }
    )
// clang-format on
{}

ruis::event_status tab_button::on_mouse_button(const ruis::mouse_button_event& event)
{
	return this->choice_button::on_mouse_button(event);
}

utki::shared_ref<ruis::touch::tab_button> ruis::touch::make::tab_button(
	utki::shared_ref<ruis::context> context, //
	ruis::touch::tab_button::all_parameters params,
	ruis::string text
)
{
	return utki::make_shared<ruis::touch::tab_button>(
		std::move(context), //
		std::move(params),
		std::move(text)
	);
}
