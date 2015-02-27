#include "BlackHoleContainer.hpp"
#include "../../util/util.hpp"


using namespace morda;



BlackHoleContainer::BlackHoleContainer(const stob::Node* chain) :
		Widget(chain),
		FrameContainer(chain)
{
	if(auto n = GetProperty(chain, "shrinkX")){
		this->shrink.x = n->AsBool();
	}else{
		this->shrink.x = true;
	}
	
	if(auto n = GetProperty(chain, "shrinkY")){
		this->shrink.y = n->AsBool();
	}else{
		this->shrink.y = true;
	}
}

Vec2r BlackHoleContainer::onMeasure(const morda::Vec2r& constraint) const {
	if(this->shrink.x && this->shrink.y){
		return this->Widget::onMeasure(constraint);
	}
	
	Vec2r ret = this->FrameContainer::onMeasure(constraint);
	
	for(unsigned i = 0; i != ret.size(); ++i){
		if(this->shrink[i] && constraint[i] < 0){
			ret[i] = 0;
		}
	}
	
	return ret;
}

