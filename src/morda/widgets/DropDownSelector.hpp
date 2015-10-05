#pragma once

#include "Widget.hpp"
#include "containers/LinearContainer.hpp"

namespace morda{

class DropDownSelector :
		virtual public Widget,
		private HorizontalContainer
{
public:
	DropDownSelector(const stob::Node* chain = nullptr);
	
	DropDownSelector(const DropDownSelector&) = delete;
	DropDownSelector& operator=(const DropDownSelector&) = delete;
private:

};

}
