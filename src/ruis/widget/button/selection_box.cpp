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

#include "selection_box.hpp"

#include "../../context.hpp"

#include "impl/nine_patch_drop_down_box.hpp"

using namespace std::string_view_literals;

using namespace ruis;

selection_box::selection_box(
	const utki::shared_ref<ruis::context>& context,
	container& selection_container,
	parameters params
) :
	widget(context, {}, {}),
	list_widget(
		context, //
		std::move(params.list)
	),
	selection_container(selection_container)
{
	// NOLINTNEXTLINE(clang-analyzer-optin.cplusplus.VirtualCall, "bypass virtual dispatch")
	this->handle_model_change();
}

void selection_box::handle_model_change()
{
	this->selection_container.clear();

	if (this->get_selection() >= this->get_provider().count()) {
		return;
	}

	this->selection_container.push_back(this->get_provider().get_highlighted_widget(this->get_selection()));
}

void selection_box::notify_selection_changed()
{
	if (this->selection_handler) {
		this->selection_handler(*this);
	}
}

void selection_box::set_selection(size_t i)
{
	this->selected_index = i;

	this->handle_model_change();
}

utki::shared_ref<ruis::selection_box> ruis::make::selection_box(
	const utki::shared_ref<ruis::context>& context, //
	ruis::selection_box::all_parameters params
)
{
	return ruis::make::nine_patch_drop_down_box(
		context, //
		{.layout_params = std::move(params.layout_params), //
		 .widget = std::move(params.widget),
		 .params{.selection_box{.list = std::move(params.params.list)}}}
	);
}
