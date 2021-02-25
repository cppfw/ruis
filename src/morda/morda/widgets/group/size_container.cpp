#include "size_container.hpp"

using namespace morda;

size_container::size_container(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		container(nullptr, desc)
{}

void size_container::lay_out(){
	for(auto& w : this->children()){
		if(w->is_layout_invalid()){
			auto d = this->dims_for_widget(*w, this->get_layout_params_const(*w));
			w->resize(d);
		}
	}
}
