#pragma once

#include "Widget.hpp"
#include "containers/LinearContainer.hpp"

namespace morda{

class DropDownList :
		virtual public Widget,
		private HorizontalContainer
{
public:
	DropDownList(const stob::Node* chain = nullptr);
	
	DropDownList(const DropDownList&) = delete;
	DropDownList& operator=(const DropDownList&) = delete;
private:

};

}
