#pragma once

#include "Button.hpp"


namespace morda{


class RadioButton : public ToggleButton{
public:
	RadioButton(const stob::Node* chain = nullptr);
	
	RadioButton(const RadioButton&) = delete;
	RadioButton& operator=(const RadioButton&) = delete;
	
private:

};

}
