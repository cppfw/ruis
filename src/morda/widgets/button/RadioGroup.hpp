#pragma once

#include "../core/container/Frame.hpp"


namespace morda{


class RadioGroup : public Frame{
public:
	RadioGroup(const stob::Node* chain = nullptr);
	
	RadioGroup(const RadioGroup&) = delete;
	RadioGroup& operator=(const RadioGroup&) = delete;
	
private:

};

}
