/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "drop_down_box.hpp"

#include "../../context.hpp"

using namespace morda;

namespace {
class static_provider : public drop_down_box::provider
{
	std::vector<treeml::tree> widgets;

public:
	size_t count() const noexcept override
	{
		return this->widgets.size();
	}

	utki::shared_ref<widget> get_widget(size_t index) override
	{
		auto i = std::next(this->widgets.begin(), index);
		ASSERT(this->get_drop_down_box())
		return this->get_drop_down_box()->context.get().inflater.inflate(i, i + 1);
	}

	void recycle(size_t index, std::shared_ptr<widget> w) override
	{
		//		TRACE(<< "static_provider::recycle(): index = " << index << std::endl)
	}

	void add(treeml::tree&& w)
	{
		this->widgets.emplace_back(std::move(w));
	}
};
} // namespace

drop_down_box::drop_down_box(
	const utki::shared_ref<morda::context>& c,
	const treeml::forest& desc,
	container& selection_container
) :
	widget(c, desc),
	selection_container(selection_container)
{
	std::shared_ptr<static_provider> pr = std::make_shared<static_provider>();

	for (const auto& p : desc) {
		if (is_property(p)) {
			continue;
		}

		pr->add(treeml::tree(p));
	}

	this->set_provider(std::move(pr));
}

void drop_down_box::set_provider(std::shared_ptr<provider> item_provider)
{
	if (item_provider && item_provider->dd) {
		throw std::logic_error("drop_down_box::setItemsProvider(): given provider is already set to some drop_down_box"
		);
	}

	if (this->item_provider) {
		this->item_provider->dd = nullptr;
	}
	this->item_provider = std::move(item_provider);
	if (this->item_provider) {
		this->item_provider->dd = this;
	}
	this->handle_data_set_changed();
}

void drop_down_box::provider::notify_data_set_changed()
{
	if (this->dd) {
		this->dd->handle_data_set_changed();
	}
}

void drop_down_box::handle_data_set_changed()
{
	this->selection_container.clear();

	if (!this->item_provider) {
		return;
	}

	if (this->get_selection() >= this->item_provider->count()) {
		return;
	}

	this->selection_container.push_back(this->item_provider->get_widget(this->get_selection()));
}

void drop_down_box::set_selection(size_t i)
{
	this->selected_index = i;

	this->handle_data_set_changed();
}
