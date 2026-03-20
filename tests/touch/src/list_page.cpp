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

#include "list_page.hpp"

#include <ruis/widget/group/touch/list.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/label/text.hpp>
#include <utki/string.hpp>
#include <utki/unicode.hpp>

#include "style.hpp"

using namespace std::string_literals;

namespace {
class list_page_provider : public ruis::list_provider
{
public:
	list_page_provider(utki::shared_ref<ruis::context> context) :
		ruis::list_provider(std::move(context))
	{}

	size_t count() const noexcept override
	{
		return 100;
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		// clang-format off
		auto button = m::push_button(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::min, ruis::length::make_pp(30)}
				}
			},
			{
				m::text(this->context, {}, U"Button"s)
			}
		);
		// clang-format on

		button.get().click_handler = [index](auto& btn) {
			std::cout << "Item #" << index << " button cliecked" << std::endl;
		};

		// clang-format off
		return m::column(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim::min}
				}
			},
			{
				m::padding(this->context,
					{
						.container_params{
							.layout = ruis::layout::row
						},
						.padding_params{
							.borders = {ruis::length::make_pp(3)}
						}
					},
					{
						m::text(this->context, {}, utki::to_utf32(utki::cat("Item #", index))),
						m::gap(this->context,
							{
								.layout_params{
									.dims = {ruis::length::make_pp(5), ruis::dim::min}
								}
							}
						),
						std::move(button)
					}
				),
				m::gap(this->context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::length::make_pp(1)}
						},
						.color_params{
							.color = 0xff808080
						}
					}
				)
			}
		);
		// clang-format on
	}
};
} // namespace

namespace {
class list_page :
	public ruis::page, //
	private ruis::touch::list
{
public:
	list_page(utki::shared_ref<ruis::context> c) :
		// clang-format off
		ruis::widget(std::move(c),
			{},
			{
				.clip = true
			}
		),
		// clang-format on
		ruis::page(this->context, {}),
		// clang-format off
		ruis::touch::list(this->context,
			{
				.oriented_params{
					.vertical = true
				},
				.list_params{
					.provider = utki::make_shared<list_page_provider>(this->context)
				}
			}
		)
	// clang-format on
	{}
};

} // namespace

utki::shared_ref<ruis::page> make_list_page(utki::shared_ref<ruis::context> c)
{
	return utki::make_shared<list_page>(std::move(c));
}
