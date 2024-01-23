/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "min_proxy.hpp"

#include "../../layouts/pile_layout.hpp"

using namespace ruis;

min_proxy::min_proxy(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
	ruis::widget(c, desc),
	ruis::container(this->context, desc, pile_layout::instance)
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		try {
			if (p.value == "root") {
				this->root_id = get_property_value(p).to_string();
			} else if (p.value == "target") {
				for (const auto& id : p.children) {
					this->target_id.push_back(id.value.to_string());
				}
			}
		} catch (std::invalid_argument&) {
			LOG([&](auto& o) {
				o << "could not parse value of " << treeml::to_string(p) << std::endl;
			})
			throw;
		}
	}
}

ruis::vector2 min_proxy::measure(const vector2& quotum) const
{
	auto t = this->target.lock();
	if (!t) {
		if (this->target_id.empty()) {
			return {0, 0};
		}

		const widget* root = [&]() {
			if (this->root_id.empty()) {
				return &this->get_root_widget();
			} else {
				return &this->get_ancestor(this->root_id.c_str());
			}
		}();
		ASSERT(root)
		for (const auto& id : this->target_id) {
			root = &root->get_widget(id, false);
		}
		this->root_id.clear();
		this->target_id.clear();
		ASSERT(root)
		this->target = utki::make_weak_from(*root);
		t = this->target.lock();
	}

	ASSERT(t)

	return t->measure(quotum);
}
