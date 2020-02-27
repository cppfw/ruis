#include "../../context.hpp"
#include "../../util/util.hpp"

#include "ImageToggle.hpp"


using namespace morda;


ImageToggle::ImageToggle(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		ToggleButton(this->context, desc),
		ImageButton(this->context, desc)
{}
