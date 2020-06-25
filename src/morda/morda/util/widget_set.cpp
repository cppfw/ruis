#include "widget_set.hpp"

using namespace morda;

void widget_set::add(std::shared_ptr<widget> w){
	this->set.insert(std::move(w));
}

void widget_set::foreach(const std::function<void(morda::widget&)>& do_this){
	if(!do_this){
		return;
	}
	for(auto& w : this->set){
		do_this(*w);
	}
}

void widget_set::set_enabled(bool enabled){
	for(auto& w : this->set){
		w->set_enabled(enabled);
	}
}
