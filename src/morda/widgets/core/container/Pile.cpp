#include "Pile.hpp"

#include "../../../util/util.hpp"

using namespace morda;


Pile::Pile(const stob::Node* chain) :
		Widget(chain),
		Container(chain)
{}



void Pile::layOut() {
//	TRACE(<< "Pile::layOut(): invoked" << std::endl)
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->getLayoutParamsDuringLayoutAs<Container::LayoutParams>(**i);
		
		(*i)->resize(this->dimForWidget(**i, lp));
		
		(*i)->moveTo(((this->rect().d - (*i)->rect().d) / 2).round());
	}
}



morda::Vec2r Pile::measure(const morda::Vec2r& quotum)const{
	Vec2r ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		utki::clampBottom(ret[i], real(0));
	}
	
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->getLayoutParamsDuringLayoutAs<Container::LayoutParams>(**i);
		
		morda::Vec2r d;
		
		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.dim[j] == LayoutParams::max_c){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = -1;
				}
			}else if(lp.dim[j] == LayoutParams::min_c){
				d[j] = -1;
			}else if(lp.dim[j] == LayoutParams::fill_c){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = 0;
				}
			}else{
				d[j] = lp.dim[j];
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

