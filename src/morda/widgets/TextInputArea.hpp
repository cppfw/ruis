#pragma once

#include "TextInputLine.hpp"
#include "List.hpp"

namespace morda{

class TextInputArea :
		private List,
		virtual public Widget
{
public:
	TextInputArea();
	
	TextInputArea(const TextInputArea&) = delete;
	TextInputArea& operator=(const TextInputArea&) = delete;
	
	
private:

};

}
