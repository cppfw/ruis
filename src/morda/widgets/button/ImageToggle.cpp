#include "../../context.hpp"
#include "../../util/util.hpp"

#include "ImageToggle.hpp"


using namespace morda;


ImageToggle::ImageToggle(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		Button(c, desc),
		ToggleButton(c, desc),
		ImageButton(c, desc)
{}
