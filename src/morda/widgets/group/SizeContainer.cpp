#include "SizeContainer.hpp"

using namespace morda;

SizeContainer::SizeContainer(const puu::forest& desc) :
		widget(desc),
		container(desc)
{}

void SizeContainer::lay_out(){
	for(auto& w : this->children()){
		if(w->is_layout_invalid()){
			auto d = this->dimForWidget(*w, this->get_layout_params(*w));
			w->resize(d);
		}
	}
}
