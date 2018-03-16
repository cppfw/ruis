#include "TextInputWrap.hpp"

using namespace morda;


TextInputWrap::TextInputWrap(const stob::Node* chain) :
		Widget(chain),
		TextWidget(chain),
		List(true, nullptr)
{
	
}
