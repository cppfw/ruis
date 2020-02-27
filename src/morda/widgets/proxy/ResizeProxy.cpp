#include "ResizeProxy.hpp"

using namespace morda;

void ResizeProxy::on_resize(){
	this->widget::on_resize();

	if(this->resize_handler){
		this->resize_handler(this->rect().d);
	}
}
