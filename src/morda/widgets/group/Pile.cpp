#include "Pile.hpp"

#include "../../util/util.hpp"

using namespace morda;


Pile::Pile(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		container(nullptr, desc)
{}



void Pile::lay_out() {
//	TRACE(<< "Pile::lay_out(): invoked" << std::endl)
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->getLayoutParamsAs<Container::LayoutParams>(**i);

		(*i)->resize(this->dimForWidget(**i, lp));

		(*i)->move_to(((this->rect().d - (*i)->rect().d) / 2).round());
	}
}



morda::Vec2r Pile::measure(const morda::Vec2r& quotum)const{
	Vec2r ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		utki::clampBottom(ret[i], real(0));
	}

	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->getLayoutParamsAs<Container::LayoutParams>(**i);

		morda::Vec2r d;

		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.dims[j] == LayoutParams::max){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = -1;
				}
			}else if(lp.dims[j] == LayoutParams::min){
				d[j] = -1;
			}else if(lp.dims[j] == LayoutParams::fill){
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

