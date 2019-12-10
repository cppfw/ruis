#include "SizeContainer.hpp"

using namespace morda;

SizeContainer::SizeContainer(const stob::Node* chain) :
		Widget(chain),
		Container(chain)
{
}

void SizeContainer::lay_out(){
	for(auto& w : this->children()){
		if(w->is_layout_invalid()){
			auto d = this->dimForWidget(*w, this->getLayoutParams(*w));
			w->resize(d);
		}
	}
}
