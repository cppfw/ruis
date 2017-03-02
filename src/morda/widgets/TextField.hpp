#pragma once

#include "TextInputLine.hpp"
#include "NinePatch.hpp"

namespace morda{


/**
 * @brief TODO:.
 */
class TextField :
		virtual public Widget,
		private NinePatch
{
	std::shared_ptr<TextInputLine> ti;
	
public:
	TextField(const stob::Node* chain = nullptr);
	
	TextField(const TextField&) = delete;
	TextField& operator=(const TextField&) = delete;
	
	TextInputLine& textInput()noexcept{
		return *this->ti;
	}
	
};

}
