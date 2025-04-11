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

#pragma once

#include "../../layout/pile_layout.hpp"
#include "../container.hpp"

namespace ruis {

/**
 * @brief container to be used for intercepting keyboard key events.
 * From GUI scripts it can be instantiated as "key_proxy".
 */
class key_proxy : public container
{
public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
	};

	key_proxy(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	key_proxy(const key_proxy&) = delete;
	key_proxy& operator=(const key_proxy&) = delete;

	key_proxy(key_proxy&&) = delete;
	key_proxy& operator=(key_proxy&&) = delete;

	~key_proxy() override = default;

	/**
	 * @brief Keyboard key signal.
	 * Emitted when a keyboard key event reaches this widget.
	 */
	std::function<bool(key_proxy& w, const ruis::key_event& e)> key_handler;

	bool on_key(const ruis::key_event& e) override;
};

namespace make {
/**
 * @brief Construct 'key_proxy' widget.
 * Default layout for children is pile.
 * @param context - ruis context.
 * @param params - 'key_proxy' widget parameters.
 * @param children - children of the 'key_proxy' widget.
 * @return newly created 'key_proxy' widget.
 */
utki::shared_ref<ruis::key_proxy> key_proxy(
	utki::shared_ref<ruis::context> context, //
	key_proxy::all_parameters params,
	widget_list children
);
} // namespace make

} // namespace ruis
