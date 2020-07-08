#include "key_proxy.hpp"

using namespace morda;

bool key_proxy::on_key(bool is_down, morda::key key_code){
	if(this->key_handler){
		return this->key_handler(*this, is_down, key_code);
	}
	return false;
}
