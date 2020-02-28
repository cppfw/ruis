#include "pile.hpp"

#include "../../util/util.hpp"

using namespace morda;

pile::pile(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		container(nullptr, desc)
{}

void pile::lay_out(){
//	TRACE(<< "pile::lay_out(): invoked" << std::endl)
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->get_layout_params_as<container::layout_params>(**i);

		(*i)->resize(this->dims_for_widget(**i, lp));

		(*i)->move_to(((this->rect().d - (*i)->rect().d) / 2).round());
	}
}

morda::vector2 pile::measure(const morda::vector2& quotum)const{
	vector2 ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		utki::clampBottom(ret[i], real(0));
	}

	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->get_layout_params_as<container::layout_params>(**i);

		morda::vector2 d;

		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.dims[j] == layout_params::max){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = -1;
				}
			}else if(lp.dims[j] == layout_params::min){
				d[j] = -1;
			}else if(lp.dims[j] == layout_params::fill){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = 0;
				}
			}else{
				d[j] = lp.dims[j];
			}
		}

		d = (*i)->measure(d);

		for(unsigned j = 0; j != d.size(); ++j){
			if(quotum[j] < 0){
				utki::clampBottom(ret[j], d[j]);
			}
		}
	}

	return ret;
}
