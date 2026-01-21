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

#include "tabbed_book.hpp"

#include "../../button/touch/tab_group.hpp"

using namespace ruis::touch;

utki::shared_ref<ruis::tabbed_book> ruis::touch::make::tabbed_book(
    utki::shared_ref<ruis::context> context, //
    ruis::tabbed_book::all_parameters params,
    std::vector< //
        std::pair<
            utki::shared_ref<ruis::choice_button>, //
            utki::shared_ref<ruis::page> //
            > //
        > //
        pages
)
{
    if (!params.tabbed_book_params.choice_group_factory) {
        // clang-format off
        params.tabbed_book_params.choice_group_factory = [](
                utki::shared_ref<ruis::context> context,
                widget_list tabs
            ){
                return ruis::touch::make::tab_group(std::move(context),
                    {
                        .layout_params{
                            .dims{ruis::dim::fill, ruis::dim::min}
                        }
                    },
                    std::move(tabs)
                );
            };
        // clang-format on

        params.tabbed_book_params.reverse_order = true;
    }

    return utki::make_shared<ruis::tabbed_book>(
        std::move(context), //
        std::move(params),
        std::move(pages)
    );
}
