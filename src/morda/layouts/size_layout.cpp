#include "size_layout.hpp"

using namespace morda;

const utki::shared_ref<size_layout> size_layout::instance = utki::make_shared<size_layout>();

void size_layout::lay_out(const vector2& size, semiconst_widget_list& widgets)const{
    // for(const auto& w : widgets){
	// 	if(w.get().is_layout_dirty()){
	// 		auto d = this->dims_for_widget(w.get(), w.get().get_layout_params_const());
	// 		w.get().resize(d);
	// 	}
	// }
}