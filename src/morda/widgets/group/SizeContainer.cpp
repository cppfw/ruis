#include "SizeContainer.hpp"

using namespace morda;

SizeContainer::SizeContainer(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		container(nullptr, desc)
{}

void SizeContainer::lay_out(){
	for(auto& w : this->children()){
		if(w->is_layout_invalid()){
			auto d = this->dims_for_widget(*w, this->get_layout_params(*w));
			w->resize(d);
		}
	}
}
