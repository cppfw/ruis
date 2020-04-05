#include "../../context.hpp"
#include "../../util/util.hpp"

#include "image_toggle.hpp"

using namespace morda;

image_toggle::image_toggle(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc),
		image_button(this->context, desc)
{}
