#include "KeyProxy.hpp"


using namespace morda;


bool KeyProxy::on_key(bool isDown, morda::Key_e keyCode) {
	if (this->key) {
		return this->key(isDown, keyCode);
	}
	return false;
}
