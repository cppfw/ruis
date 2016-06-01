#include "BlackHoleContainer.hpp"
#include "../../util/util.hpp"


using namespace morda;



BlackHoleContainer::BlackHoleContainer(const stob::Node* chain) :
		Widget(chain),
		FrameContainer(chain)
{
	if(auto n = getProperty(chain, "shrinkX")){
		this->shrink.x = n->asBool();
	}else{
		this->shrink.x = true;
	}
	
	if(auto n = getProperty(chain, "shrinkY")){
		this->shrink.y = n->asBool();
	}else{
		this->shrink.y = true;
	}
}

Vec2r BlackHoleContainer::measure(const morda::Vec2r& quotum) const {
	if(this->shrink.x && this->shrink.y){
		return this->Widget::measure(quotum);
	}
	
	Vec2r ret = this->FrameContainer::measure(quotum);
	
	for(unsigned i = 0; i != ret.size(); ++i){
		if(this->shrink[i] && quotum[i] < 0){
			ret[i] = 0;
		}
	}
	
	return ret;
}

