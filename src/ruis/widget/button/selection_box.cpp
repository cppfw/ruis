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

#include "selection_box.hpp"

#include "../../context.hpp"

#include "impl/drop_down_box.hpp"

using namespace std::string_view_literals;

using namespace ruis;

namespace {
// TODO: remove?
class static_provider : public list_provider
{
	std::vector<tml::tree> widgets;

public:
	static_provider(utki::shared_ref<ruis::context> context) :
		list_provider(std::move(context))
	{}

	size_t count() const noexcept override
	{
		return this->widgets.size();
	}

	utki::shared_ref<widget> get_widget(size_t index) override
	{
		auto i = utki::next(this->widgets.begin(), index);
		return this->context.get().inflater.inflate(i, i + 1);
	}

	void recycle(size_t index, utki::shared_ref<widget> w) override
	{
		//		TRACE(<< "static_provider::recycle(): index = " << index << std::endl)
	}

	void add(tml::tree w)
	{
		this->widgets.emplace_back(std::move(w));
	}
};
} // namespace

selection_box::selection_box(
	utki::shared_ref<ruis::context> context,
	container& selection_container,
	list_providable::parameters providable_params
) :
	widget(std::move(context), {}, {}),
	list_providable(std::move(providable_params)),
	selection_container(selection_container)
{
	this->handle_model_change();
}

selection_box::selection_box(
	const utki::shared_ref<ruis::context>& c,
	const tml::forest& desc,
	container& selection_container
) :
	widget(c, desc),
	list_providable({}),
	selection_container(selection_container)
{
	std::shared_ptr<static_provider> pr = std::make_shared<static_provider>(c);

	for (const auto& p : desc) {
		if (is_property(p)) {
			continue;
		}

		pr->add(tml::tree(p));
	}

	this->set_provider(std::move(pr));
}

void selection_box::handle_model_change()
{
	this->selection_container.clear();

	if (!this->get_provider()) {
		return;
	}

	if (this->get_selection() >= this->get_provider()->count()) {
		return;
	}

	this->selection_container.push_back(this->get_provider()->get_widget(this->get_selection()));
}

void selection_box::set_selection(size_t i)
{
	this->selected_index = i;

	this->handle_model_change();
}

utki::shared_ref<ruis::selection_box> ruis::make::selection_box(
	utki::shared_ref<ruis::context> context, //
	ruis::selection_box::all_parameters params
)
{
	auto& c = context.get();
	return utki::make_shared<ruis::drop_down_box>(
		std::move(context), //
		// clang-format off
		ruis::drop_down_box::all_parameters{
			.layout_params = std::move(params.layout_params), //
			.widget_params = std::move(params.widget_params),
			.nine_patch_button_params = {
				.unpressed_nine_patch = c.loader.load<res::nine_patch>("ruis_npt_button_normal"sv),
				.pressed_nine_patch = c.loader.load<res::nine_patch>("ruis_npt_button_pressed"sv)
			},
			.providable_params = std::move(params.providable_params)
		} // clang-format on
	);
}

void selection_box::on_reload()
{
	if (this->get_provider()) {
		this->get_provider()->on_reload();
	}
}
