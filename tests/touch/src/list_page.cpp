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
#include <ruis/widget/label/text.hpp>

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
		return m::text(this->context, {}, U"Hello world!"s);
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
