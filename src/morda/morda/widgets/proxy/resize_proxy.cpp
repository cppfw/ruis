#include "resize_proxy.hpp"

using namespace morda;

void resize_proxy::on_resize(){
	this->widget::on_resize();

	if(this->resize_handler){
		this->resize_handler(*this);
	}
}
