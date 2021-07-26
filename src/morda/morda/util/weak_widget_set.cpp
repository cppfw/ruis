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

#include "weak_widget_set.hpp"

using namespace morda;

void weak_widget_set::add(std::weak_ptr<widget> w){
	this->set.insert(std::move(w));
}

void weak_widget_set::foreach(const std::function<void(morda::widget&)>& do_this){
	if(!do_this){
		return;
	}
	for(auto& wdgt : this->set){
		if(auto w = wdgt.lock()){
			do_this(*w);
		}
	}
}

void weak_widget_set::set_enabled(bool enabled){
	for(auto& wdgt : this->set){
		if(auto w = wdgt.lock()){
			w->set_enabled(enabled);
		}
	}
}
