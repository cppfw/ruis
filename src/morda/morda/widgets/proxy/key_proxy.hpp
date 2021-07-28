/*
morda GUI framework

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#include "../group/pile.hpp"

namespace morda{

/**
 * @brief container to be used for intercepting keyboard key events.
 * From GUI scripts it can be instantiated as "key_proxy".
 */
class key_proxy : public pile{
public:
	key_proxy(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc),
			pile(this->context, desc)
	{}
	
	key_proxy(const key_proxy&) = delete;
	key_proxy& operator=(const key_proxy&) = delete;
	
	/**
	 * @brief Keyboard key signal.
	 * Emitted when a keyboard key event reaches this widget.
	 */
	std::function<bool(key_proxy& w, bool is_down, morda::key key_code)> key_handler;
	
	virtual bool on_key(bool is_down, morda::key key_code)override;
};

}
