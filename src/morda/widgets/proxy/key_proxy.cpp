#include "key_proxy.hpp"

using namespace morda;

bool key_proxy::on_key(bool isDown, morda::key keyCode){
	if(this->key_handler){
		return this->key_handler(isDown, keyCode);
	}
	return false;
}
