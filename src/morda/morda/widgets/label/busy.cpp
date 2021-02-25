#include "busy.hpp"

using namespace morda;

busy::busy(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		spinner(this->context, desc)
{
	if(!this->get_image()){
		this->set_image(this->context->loader.load<res::image>("morda_img_busy"));
	}
}

void busy::set_active(bool active){
	this->set_visible(active);
	this->spinner::set_active(active);
}
