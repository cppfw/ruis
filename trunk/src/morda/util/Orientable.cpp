#include "Orientable.hpp"
#include "../util/util.hpp"


using namespace morda;



Orientable::Orientable(const stob::Node* chain){
	if(const stob::Node* n = GetProperty(chain, "vertical")){
		this->isVertical = n->AsBool();
	}else{
		this->isVertical = false;
	}
}


