#include "CharInputFocusable.hpp"

#include "../App.hpp"

using namespace morda;



void CharInputFocusable::FocusCharInput()const{
	App::Inst().focusedCharInput = this->SharedFromThis(const_cast<CharInputFocusable*>(this));
}



void CharInputFocusable::UnfocusCharInput()const{
	if(auto c = App::Inst().focusedCharInput.lock()){
		if(c.operator->() == this){
			App::Inst().focusedCharInput.reset();
		}
	}
}

