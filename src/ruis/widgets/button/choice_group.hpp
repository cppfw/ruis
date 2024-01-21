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

#pragma once

#include "../../container.hpp"

#include "choice_button.hpp"

namespace ruis {

/**
 * @brief Choice group container.
 * This is a container which is used to group several choice buttons to the same
 * choice group.
 * In the GUI script can be instantiated as "choice_group". Only available after
 * initializing standard morda widgets.
 */
class choice_group : public container
{
	friend class choice_button;

	std::weak_ptr<choice_button> active_choice_button;

public:
	choice_group(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc);

	choice_group(const choice_group&) = delete;
	choice_group& operator=(const choice_group&) = delete;

	choice_group(choice_group&&) = delete;
	choice_group& operator=(choice_group&&) = delete;

	~choice_group() override = default;

	bool is_active(const widget& w) const noexcept;

	std::weak_ptr<choice_button> get_active() const noexcept
	{
		return this->active_choice_button;
	}

private:
	void set_active_choice_button(std::weak_ptr<choice_button> rb);
};

} // namespace ruis
