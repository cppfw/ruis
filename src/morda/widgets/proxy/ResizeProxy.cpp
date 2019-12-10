#include "ResizeProxy.hpp"


using namespace morda;




void ResizeProxy::on_resize() {
	this->Widget::on_resize();

	if (this->resized) {
		this->resized(this->rect().d);
	}
}


