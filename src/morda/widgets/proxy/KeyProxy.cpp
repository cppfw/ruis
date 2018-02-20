#include "KeyProxy.hpp"


using namespace morda;


bool KeyProxy::onKey(bool isDown, morda::Key_e keyCode) {
	if (this->key) {
		return this->key(isDown, keyCode);
	}
	return false;
}
