#include "layout.hpp"

using namespace morda;

vector2 layout::measure(const vector2& quotum, const_widget_list& widgets)const{
    return max(quotum, 0);
}

void layout::lay_out(const vector2& size, semiconst_widget_list& widgets)const{
    for(auto& w : widgets){
		if(w.get().is_layout_dirty()){
			w.get().layout_dirty = false;
			w.get().lay_out();
		}
	}
}
