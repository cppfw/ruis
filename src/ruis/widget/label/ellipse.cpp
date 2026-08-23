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

#include "ellipse.hpp"

using namespace ruis;

ellipse::ellipse( //
    utki::shared_ref<ruis::context> context,
    all_parameters params,
    widget_list children
) :
    widget(
        std::move(context), //
        std::move(params.layout_params),
        std::move(params.widget_params)
    ),
    // clang-format off
    padding(
        this->context, //
        {
            .container_params = std::move(params.container_params),
            .padding_params = std::move(params.padding_params)
        },
        std::move(children)
    ),
    // clang-format on
    color_widget(
        this->context, //
        std::move(params.color_params)
    ),
    vao(this->context.get().renderer)
{
    this->update_vao();
}

void ellipse::update_vao()
{
    this->vao.set(
        this->rect().d / 2, //
        0 // stroke width
    );
}

void ellipse::on_resize()
{
    this->update_vao();

    this->padding::on_resize();
}

void ellipse::render(const ruis::mat4& matrix) const
{
    this->vao.render(
        matrix, //
        this->get_current_color()
    );

    this->padding::render(matrix);
}

utki::shared_ref<ruis::ellipse> ruis::make::ellipse(
    utki::shared_ref<ruis::context> context, //
    ruis::ellipse::all_parameters params,
    widget_list children
)
{
    return utki::make_shared<ruis::ellipse>(
        std::move(context), //
        std::move(params),
        std::move(children)
    );
}
