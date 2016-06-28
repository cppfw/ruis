/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "../container/FrameContainer.hpp"

namespace morda{

class KeyProxy : public FrameContainer{
public:
	KeyProxy(const stob::Node* chain = nullptr) :
			Widget(chain),
			FrameContainer(chain)
	{}
	
	KeyProxy(const KeyProxy&) = delete;
	KeyProxy& operator=(const KeyProxy&) = delete;
	
	std::function<bool(bool isDown, morda::EKey keyCode)> key;
	
	virtual bool onKey(bool isDown, morda::EKey keyCode)override{
		if(this->key){
			return this->key(isDown, keyCode);
		}
		return false;
	}
};

}
