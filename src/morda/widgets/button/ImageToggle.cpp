#include "../../Morda.hpp"
#include "../../util/util.hpp"

#include "ImageToggle.hpp"


using namespace morda;


ImageToggle::ImageToggle(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		ToggleButton(chain),
		ImageButton(chain)
{	
}
