#include "WrappedTextInput.hpp"

using namespace morda;


WrappedTextInput::WrappedTextInput(const stob::Node* chain) :
		Widget(chain),
		TextWidget(chain),
		List(true, nullptr)
{
	
}
