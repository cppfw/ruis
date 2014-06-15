#include "CharInputFocusable.hpp"

#include "../App.hpp"

using namespace morda;



void CharInputFocusable::FocusCharInput()const{
	App::Inst().focusedCharInput = const_cast<CharInputFocusable*>(this);
}



void CharInputFocusable::UnfocusCharInput()const{
	if(ting::Ref<CharInputFocusable> c = App::Inst().focusedCharInput){
		if(c.operator->() == this){
			App::Inst().focusedCharInput.Reset();
		}
	}
}

