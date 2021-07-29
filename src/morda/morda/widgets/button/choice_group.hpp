/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

#include "choice_button.hpp"

namespace morda{

/**
 * @brief Choice group container.
 * This is a container which is used to group several choice buttons to the same
 * choice group.
 * In the GUI script can be instantiated as "choice_group". Only available after
 * initializing standard morda widgets.
 */
class choice_group : public pile{
	friend class choice_button;
	
	std::shared_ptr<choice_button> activeChoiceButton_v;
public:
	choice_group(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
	choice_group(const choice_group&) = delete;
	choice_group& operator=(const choice_group&) = delete;
	
	bool is_active(const widget& w)const noexcept;
	
	const decltype(activeChoiceButton_v)& get_active()const noexcept{
		return this->activeChoiceButton_v;
	}
	
private:
	void setActiveChoiceButton(const std::shared_ptr<choice_button>& rb);
};

}
