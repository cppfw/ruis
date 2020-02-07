#include "../../context.hpp"
#include "../../util/util.hpp"

#include "ImageToggle.hpp"


using namespace morda;


ImageToggle::ImageToggle(const puu::forest& desc) :
		widget(desc),
		Button(desc),
		ToggleButton(desc),
		ImageButton(desc)
{}
