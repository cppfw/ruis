#include "gui.hpp"

#include "res/puu.hpp"

#include "widgets/slider/scroll_bar.hpp"

#include "widgets/button/nine_patch_push_button.hpp"
#include "widgets/button/check_box.hpp"
#include "widgets/button/radio_button.hpp"
#include "widgets/button/choice_group.hpp"
#include "widgets/button/image_toggle.hpp"
#include "widgets/button/image_push_button.hpp"
#include "widgets/button/tab.hpp"
#include "widgets/button/tab_group.hpp"
#include "widgets/button/click_drop_down_box.hpp"

#include "widgets/label/busy.hpp"
#include "widgets/label/color.hpp"
#include "widgets/label/text.hpp"
#include "widgets/label/gradient.hpp"
#include "widgets/label/mouse_cursor.hpp"

#include "widgets/input/text_input_line.hpp"

#include "widgets/group/tree_view.hpp"
#include "widgets/group/window.hpp"
#include "widgets/group/collapse_area.hpp"

using namespace morda;

gui::gui(
		std::shared_ptr<morda::renderer> r,
        std::shared_ptr<morda::updater> u,
		std::function<void(std::function<void()>&&)>&& run_from_ui_thread_function,
		real dots_per_inch,
		real dots_per_dp
	) :
		context(std::make_shared<morda::context>(
				std::move(r),
                std::move(u),
				std::move(run_from_ui_thread_function),
				dots_per_inch,
				dots_per_dp
			))
{
	ASSERT(this->context)
}

void gui::initStandardWidgets(papki::file& fi) {

	// mount default resource pack

	std::vector<std::string> paths;

	if(fi.path().length() != 0){
		paths.push_back(fi.path());
	}

	paths.push_back("morda_res/");

#if (M_OS == M_OS_LINUX && M_OS_NAME != M_OS_NAME_ANDROID) || \
	(M_OS == M_OS_MACOSX && M_OS_NAME != M_OS_NAME_IOS) || \
	(M_OS == M_OS_UNIX)

	unsigned soname =
#	include "../../soname.txt"
	;

	{
		std::stringstream ss;
		ss << "/usr/local/share/morda/res" << soname << "/";
		paths.push_back(ss.str());
	}
	{
		std::stringstream ss;
		ss << "/usr/share/morda/res" << soname << "/";
		paths.push_back(ss.str());
	}
#endif

	bool mounted = false;
	for(const auto& s : paths){
		try{
//			TRACE(<< "s = " << s << std::endl)
			fi.set_path(s);
//			TRACE(<< "fi.path() = " << fi.path() << std::endl)
			this->context->loader.mount_res_pack(fi);
		}catch(std::runtime_error&){
//			TRACE(<< "could not mount " << s << std::endl)
			continue;
		}
		mounted = true;
		break;
	}

	if(!mounted){
		throw std::runtime_error("gui::initStandardWidgets(): could not mount default resource pack");
	}

	// add standard widgets to inflater

	this->context->inflater.register_widget<text>("text");
	this->context->inflater.register_widget<color>("color");
	this->context->inflater.register_widget<gradient>("gradient");
	this->context->inflater.register_widget<image>("image");
	this->context->inflater.register_widget<vertical_scroll_bar>("vertical_scroll_bar");
	this->context->inflater.register_widget<horizontal_scroll_bar>("horizontal_scroll_bar");
	this->context->inflater.register_widget<window>("window");
	this->context->inflater.register_widget<nine_patch>("nine_patch");
	this->context->inflater.register_widget<nine_patch_push_button>("nine_patch_push_button");
	this->context->inflater.register_widget<check_box>("check_box");
	this->context->inflater.register_widget<tree_view>("tree_view");
	this->context->inflater.register_widget<click_drop_down_box>("click_drop_down_box");
	this->context->inflater.register_widget<radio_button>("radio_button");
	this->context->inflater.register_widget<choice_group>("choice_group");
	this->context->inflater.register_widget<mouse_cursor>("mouse_cursor");
	this->context->inflater.register_widget<collapse_area>("collapse_area");
	this->context->inflater.register_widget<image_toggle>("image_toggle");
	this->context->inflater.register_widget<image_push_button>("image_push_button");
	this->context->inflater.register_widget<tab_group>("tab_group");
	this->context->inflater.register_widget<tab>("tab");
	this->context->inflater.register_widget<text_input_line>("text_input_line");
	this->context->inflater.register_widget<busy>("busy");

	try{
		auto t = this->context->loader.load<res::puu>("morda_gui_defs");

		this->context->inflater.inflate(t->forest());

	}catch(std::exception&){
		//TODO: do not ignore?
		TRACE(<< "gui::initStandardWidgets(): could not load morda_gui_definitions" << std::endl)
	}
}

void gui::set_viewport(const morda::vector2& size){
	this->viewportSize = size;

	if(!this->root_widget){
		return;
	}

	this->root_widget->resize(this->viewportSize);
}

void gui::set_root(std::shared_ptr<morda::widget> w){
	this->root_widget = std::move(w);

	this->root_widget->move_to(morda::vector2(0));
	this->root_widget->resize(this->viewportSize);
}

void gui::render(const matrix4& matrix)const{
	if(!this->root_widget){
		TRACE(<< "gui::render(): root widget is not set" << std::endl)
		return;
	}

	morda::matrix4 m(matrix);

	// direct y-axis down
	m.scale(1, -1);

	m.translate(-1, -1);
	m.scale(vector2(2).comp_divide(this->viewportSize));

	ASSERT(this->root_widget)

	if(this->root_widget->is_layout_invalid()){
		TRACE(<< "root widget re-layout needed!" << std::endl)
		this->root_widget->relayoutNeeded = false;
		this->root_widget->lay_out();
	}

	this->root_widget->renderInternal(m);
}

void gui::send_mouse_move(const vector2& pos, unsigned id){
	if(!this->root_widget){
		return;
	}

	if(this->root_widget->is_interactive()){
		this->root_widget->set_hovered(this->root_widget->rect().overlaps(pos), id);
		this->root_widget->on_mouse_move(mouse_move_event{
				pos,
				id
			});
	}
}

void gui::send_mouse_button(bool is_down, const vector2& pos, mouse_button button, unsigned id){
	if(!this->root_widget){
		return;
	}

	if(this->root_widget->is_interactive()){
		this->root_widget->set_hovered(this->root_widget->rect().overlaps(pos), id);
		this->root_widget->on_mouse_button(mouse_button_event{
				is_down,
				pos,
				button,
				id
			});
	}
}

void gui::send_mouse_hover(bool isHovered, unsigned pointerID){
	if(!this->root_widget){
		return;
	}

	this->root_widget->set_hovered(isHovered, pointerID);
}

void gui::send_key(bool is_down, key key_code){
//		TRACE(<< "HandleKeyEvent(): is_down = " << is_down << " is_char_input_only = " << is_char_input_only << " keyCode = " << unsigned(keyCode) << std::endl)

	if(auto w = this->context->focused_widget.lock()){
//		TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->onKeyInternal(is_down, key_code);
	}else{
//		TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		if(this->root_widget){
			this->root_widget->onKeyInternal(is_down, key_code);
		}
	}
}

void gui::send_character_input(const unicode_provider& unicode, key key_code){
	if(auto w = this->context->focused_widget.lock()){
		//			TRACE(<< "HandleCharacterInput(): there is a focused widget" << std::endl)
		if(auto c = dynamic_cast<character_input_widget*>(w.get())){
			c->on_character_input(unicode.get(), key_code);
		}
	}
}
