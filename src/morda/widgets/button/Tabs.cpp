#include "../../Exc.hpp"

#include "Tab.hpp"
#include "Tabs.hpp"


using namespace morda;

Tabs::Tabs(const stob::Node* chain) :
		Widget(chain),
		ChoiceGroup(chain)
{
	
}

morda::Vec2r Tabs::measure(const morda::Vec2r& quotum) const {
	Vec2r ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		utki::clampBottom(ret[i], real(0));
	}
	
	real length = 0;
	
	Tab* prev = nullptr;
	
	for(auto b = this->children().begin(), e = this->children().end(), i = b; i != e; ++i){
		auto& c = *i;
		ASSERT(c)
		auto& lp = this->getLayoutParamsDuringLayoutAs<Container::LayoutParams>(*c);
		
		auto tab = dynamic_cast<Tab*>(c.get());
		if(!tab){
			throw morda::Exc("Non-Tab widget added to Tabs, only Tab widgets are allowed to be added to Tabs");
		}
		
		morda::Vec2r d;
		
		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.dim[j] == LayoutParams::max_c || lp.dim[j] == LayoutParams::fill_c){
				throw LayoutExc("'max' or 'fill' encountered in layout parameters for Tabs container");
			}else if(lp.dim[j] == LayoutParams::min_c){
				d[j] = -1;
			}else{
				d[j] = lp.dim[j];
			}
		}
		
		d = c->measure(d);
		
		length += d.x;
		
		if(quotum.y < 0){
			utki::clampBottom(ret.y, d.y);
		}
	}
	
	if(quotum.x < 0){
		ret.x = length;
	}
	
	return ret;
}

void Tabs::layOut() {
	real pos = 0;
	for(auto& c : this->children()){
		ASSERT(c)
		auto& lp = this->getLayoutParamsDuringLayoutAs<Container::LayoutParams>(*c);
		
		auto dim = this->dimForWidget(*c, lp);
		c->resize(dim);
		
		c->moveTo(Vec2r(pos, std::round((this->rect().d.y - c->rect().d.y) / 2)));
		pos += dim.x;
	}
}
