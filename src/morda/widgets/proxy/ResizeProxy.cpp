#include "ResizeProxy.hpp"


using namespace morda;




void ResizeProxy::on_resize() {
	this->widget::on_resize();

	if (this->resized) {
		this->resized(this->rect().d);
	}
}


