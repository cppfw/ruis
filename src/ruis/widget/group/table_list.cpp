/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "table_list.hpp"

#include "tiling_area.hpp"

using namespace ruis;

namespace ruis::internal {
class provider : public list_provider
{
public:
	table_list& owner;

	const utki::shared_ref<table_list::provider> table_list_provider;

	provider(
		table_list& owner, //
		utki::shared_ref<table_list::provider> table_list_provider
	) :
		list_provider(table_list_provider.get().context),
		owner(owner),
		table_list_provider(std::move(table_list_provider))
	{
		this->table_list_provider.get().model_change_signal.connect([this]() {
			this->notify_model_change();
		});
	}

	size_t count() const noexcept override
	{
		return this->table_list_provider.get().count();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		auto cells = this->table_list_provider.get().get_row_widgets(index);

		this->owner.arrange_list_item_cells(cells);

		// clang-format off
		return make::container(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::max, ruis::dim::min}
				},
				.container_params{
					.layout = ruis::layout::trivial
				}
			},
			std::move(cells)
		);
		// clang-format on
	}
};
} // namespace ruis::internal

namespace {
utki::shared_ref<ruis::tiling_area> make_headers_widget(
	const utki::shared_ref<ruis::context>& c, //
	ruis::widget_list column_headers
)
{
	// clang-format off
	return ruis::make::tiling_area(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::min}
			}
		},
		std::move(column_headers)
	);
	// clang-format on
}
} // namespace

table_list::table_list(
	utki::shared_ref<ruis::context> context, //
	all_parameters params
) :
	table_list(
		make_headers_widget(
			context, //
			std::move(params.table_list_params.column_headers)
		),
		// clang-format off
		ruis::make::list(context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim::fill},
					.weight = 1
				},
				.widget_params{
					.clip = true
				},
				.list_params{
					.provider = [&]() -> utki::shared_ref<list_provider> {
						return utki::make_shared<internal::provider>(
							*this,
							std::move(params.table_list_params.provider)
						);
					}()
				}
			}
		),
		// clang-format on
		params
	)
{}

table_list::table_list(
	utki::shared_ref<tiling_area> headers, //
	utki::shared_ref<list> rows_list,
	all_parameters& params
) :
	ruis::widget(
		headers.get().context, //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	ruis::container(
		this->context,
		{
			.container_params{
				.layout = ruis::layout::column
			}
		},
		{
			headers,
			rows_list
		}
	),
	// clang-format on
	headers_tiling_area(std::move(headers)),
	table_rows_list(std::move(rows_list))
{
	this->headers_tiling_area.get().tiles_resized_handler = [this](auto& ta) {
		for (auto& child : this->table_rows_list.get().children()) {
			auto* c = dynamic_cast<ruis::container*>(&child.get());
			ASSERT(c)
			this->arrange_list_item_cells(c->children());
		}
	};

	this->table_rows_list.get().model_change_handler = [this](auto& l) {
		this->notify_model_changed();
	};

	this->table_rows_list.get().scroll_change_handler = [this](auto& l) {
		this->notify_scroll_changed();
	};
}

void table_list::notify_model_changed()
{
	if (this->model_change_handler) {
		this->model_change_handler(*this);
	}
}

void table_list::notify_scroll_changed()
{
	if (this->scroll_change_handler) {
		this->scroll_change_handler(*this);
	}
}

void table_list::arrange_list_item_cells(ruis::semiconst_widget_list& cells)
{
	real pos = 0;
	for (auto [c, h] : utki::views::zip(cells, this->headers_tiling_area.get().content().children())) {
		auto md = c.get().measure({
			h.get().rect().d.x(),
			-1 //
		});
		c.get().resize(md);
		c.get().move_to({
			pos,
			0 //
		});
		pos += md.x();
	}
}

void table_list::provider::notify_model_change()
{
	this->context.get().post_to_ui_thread([this]() {
		this->model_change_signal.emit();
	});
}

utki::shared_ref<ruis::table_list> make::table_list(
	utki::shared_ref<ruis::context> context, //
	ruis::table_list::all_parameters params
)
{
	return utki::make_shared<ruis::table_list>(
		std::move(context), //
		std::move(params)
	);
}
