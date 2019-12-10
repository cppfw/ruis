#include "KeyProxy.hpp"


using namespace morda;


bool KeyProxy::on_key(bool isDown, morda::key keyCode) {
	if (this->key) {
		return this->key(isDown, keyCode);
	}
	return false;
}
