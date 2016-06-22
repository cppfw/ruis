/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "../container/FrameContainer.hpp"

namespace morda{

class KeyProxyContainer : public FrameContainer{
public:
	KeyProxyContainer(const stob::Node* chain = nullptr) :
			Widget(chain),
			FrameContainer(chain)
	{}
	
	KeyProxyContainer(const KeyProxyContainer&) = delete;
	KeyProxyContainer& operator=(const KeyProxyContainer&) = delete;
	
	std::function<bool(bool isDown, morda::EKey keyCode)> key;
	
	virtual bool onKey(bool isDown, morda::EKey keyCode)override{
		if(this->key){
			return this->key(isDown, keyCode);
		}
		return false;
	}
};

}
