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

#include "table_tree_view.hpp"

using namespace ruis;

class table_tree_view::table_list_provider_for_table_tree_view : public table_list::provider
{
	utki::shared_ref<table_tree_view::provider> provider;

public:
	table_list_provider_for_table_tree_view(
		utki::shared_ref<ruis::context> context, //
		utki::shared_ref<table_tree_view::provider> provider
	) :
		table_list::provider(std::move(context)),
		provider(std::move(provider))
	{
		if (this->provider.get().list_provider) {
			throw std::invalid_argument(
				"table_tree_view::table_tree_view(): the passed in provider is already added to another table_tree_view"
			);
		}
		this->provider.get().list_provider = this;
		this->provider.get().init();
	}

	size_t count() const noexcept override
	{
		return this->provider.get().list_count();
	}

	widget_list get_row_widgets(size_t index) override
	{
		return this->provider.get().list_get_row_widgets(index);
	}
};

table_tree_view::provider::provider(utki::shared_ref<ruis::context> context) :
	provider_base(std::move(context))
{}

void table_tree_view::provider::on_list_model_changed()
{
	if (this->list_provider) {
		this->list_provider->notify_model_change();
	}
}

ruis::widget_list table_tree_view::provider::list_get_row_widgets(size_t index)
{
	auto parts = this->get_item_widget_parts(index);

	auto wl = this->get_row_widgets(parts.index);

	if (wl.empty()) {
		return wl;
	}

	parts.prefix_widgets.push_back(wl.front());

	wl.front() = ruis::make::row(
		this->context, //
		{.widget_params{.clip = true}},
		std::move(parts.prefix_widgets)
	);
	return wl;
}

table_tree_view::table_tree_view(
	utki::shared_ref<ruis::context> context, //
	all_parameters params
) :
	ruis::widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
    ruis::table_list(
        this->context,
        {
            .table_list_params{
                .column_headers = std::move(params.table_tree_view_params.column_headers),
                .provider = utki::make_shared<table_tree_view::table_list_provider_for_table_tree_view>(
                    this->context,
                    std::move(params.table_tree_view_params.provider)
                )
            }
        }
    )
// clang-format on
{}

utki::shared_ref<ruis::table_tree_view> make::table_tree_view(
	utki::shared_ref<ruis::context> context, //
	ruis::table_tree_view::all_parameters params
)
{
	return utki::make_shared<ruis::table_tree_view>(
		std::move(context), //
		std::move(params)
	);
}
