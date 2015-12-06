/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "TextInput.hpp"
#include "NinePatch.hpp"

namespace morda{



class TextField :
		virtual public Widget,
		private NinePatch
{
	std::shared_ptr<TextInput> ti;
	
public:
	TextField(const stob::Node* chain = nullptr);
	
	TextField(const TextField&) = delete;
	TextField& operator=(const TextField&) = delete;
	
	TextInput& textInput()noexcept{
		return *this->ti;
	}
	
};

}
