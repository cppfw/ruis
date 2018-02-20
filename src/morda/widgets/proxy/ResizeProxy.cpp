#include "ResizeProxy.hpp"


using namespace morda;




void ResizeProxy::onResize() {
	this->Widget::onResize();

	if (this->resized) {
		this->resized(this->rect().d);
	}
}


