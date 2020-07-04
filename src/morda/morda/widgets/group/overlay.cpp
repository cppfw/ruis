#include "overlay.hpp"
#include "../proxy/mouse_proxy.hpp"

#include "../../context.hpp"
#include "size_container.hpp"

using namespace morda;

namespace{
class context_menu_wrapper : public size_container{
public:
	context_menu_wrapper(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			size_container(this->context, desc)
	{}
};
}

overlay::overlay(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		pile(this->context, desc)
{}

std::shared_ptr<widget> overlay::show_context_menu(std::shared_ptr<widget> w, vector2 anchor){
	auto c = std::make_shared<context_menu_wrapper>(this->context, puu::read(R"qwertyuiop(
		layout{
			dx{fill} dy{fill}
		}
		@mouse_proxy{
			layout{
				dx{fill} dy{fill}
			}
			x{0} y{0}
		}
	)qwertyuiop"));

	auto& mp = *std::dynamic_pointer_cast<mouse_proxy>(c->children().back());

	mp.mouse_button_handler = [](widget& w, bool is_down, const vector2& pos, mouse_button button, unsigned pointer_id) -> bool{
		ASSERT(w.parent())
		auto wsp = utki::make_shared_from_this(*w.parent());
		w.context->run_from_ui_thread([wsp](){
			wsp->remove_from_parent();
		});
		return false;
	};

	c->push_back(w);

	auto& lp = c->get_layout_params_const(*w);

	vector2 dim = this->dims_for_widget(*w, lp);

	for(unsigned i = 0; i != 2; ++i){
		dim[i] = std::min(dim[i], this->rect().d[i]);
	}

	w->resize(dim);

	for(unsigned i = 0; i != 2; ++i){
		anchor[i] = std::max(anchor[i], real(0)); // clamp bottom
		anchor[i] = std::min(anchor[i], this->rect().d[i] - w->rect().d[i]); // clamp top
	}

	w->move_to(anchor);

	auto sp = utki::make_shared_from_this(*this);
	ASSERT(sp)
	this->context->run_from_ui_thread([c, sp](){
		sp->push_back(c);
	});

	return c;
}

void overlay::close_all_context_menus(){
	while(dynamic_cast<context_menu_wrapper*>(this->children().back().get())){
		this->pop_back();
	}
}
