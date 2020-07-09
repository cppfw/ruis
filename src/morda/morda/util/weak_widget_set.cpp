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
