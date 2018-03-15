#pragma once

#include "../Widget.hpp"
#include "../group/List.hpp"
#include "../base/TextWidget.hpp"

namespace morda{

class WrappedTextInput :
		public virtual Widget,
		public TextWidget,
		private List
{
public:
	WrappedTextInput(const stob::Node* chain);
	
	WrappedTextInput(const WrappedTextInput&) = delete;
	WrappedTextInput& operator=(const WrappedTextInput&) = delete;
	
private:

};

}
