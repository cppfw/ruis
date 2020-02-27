#include "gui.hpp"

#include "res/res_puu.hpp"

#include "widgets/slider/ScrollBar.hpp"

#include "widgets/button/NinePatchPushButton.hpp"
#include "widgets/button/check_box.hpp"
#include "widgets/button/RadioButton.hpp"
#include "widgets/button/choice_group.hpp"
#include "widgets/button/image_toggle.hpp"
#include "widgets/button/image_push_button.hpp"
#include "widgets/button/Tab.hpp"
#include "widgets/button/Tabs.hpp"
#include "widgets/button/drop_down_box.hpp"

#include "widgets/label/Color.hpp"
#include "widgets/label/Text.hpp"
#include "widgets/label/Gradient.hpp"
#include "widgets/label/MouseCursor.hpp"

#include "widgets/input/TextInputLine.hpp"

#include "widgets/group/TreeView.hpp"
#include "widgets/group/Window.hpp"
#include "widgets/group/CollapseArea.hpp"

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

void gui::initStandardWidgets(papki::File& fi) {

	//mount default resource pack

	std::vector<std::string> paths;

	if(fi.path().length() != 0){
		paths.push_back(fi.path());
	}

	paths.push_back("morda_res/");

#if (M_OS == M_OS_LINUX && M_OS_NAME != M_OS_NAME_ANDROID) || \
	(M_OS == M_OS_MACOSX && M_OS_NAME != M_OS_NAME_IOS) || \
	(M_OS == M_OS_UNIX)

	unsigned soname =
#	include "../soname.txt"
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
			fi.setPath(s);
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

	this->context->inflater.register_widget<Text>("text");
	this->context->inflater.register_widget<Color>("color");
	this->context->inflater.register_widget<Gradient>("gradient");
	this->context->inflater.register_widget<Image>("image");
	this->context->inflater.register_widget<VScrollBar>("vertical_scroll_bar");
	this->context->inflater.register_widget<HScrollBar>("horizontal_scroll_bar");
	this->context->inflater.register_widget<Window>("window");
	this->context->inflater.register_widget<NinePatch>("nine_patch");
	this->context->inflater.register_widget<NinePatchPushButton>("nine_patch_push_button");
	this->context->inflater.register_widget<check_box>("check_box");
	this->context->inflater.register_widget<TreeView>("tree_view");
	this->context->inflater.register_widget<drop_down_box>("drop_down_box");
	this->context->inflater.register_widget<RadioButton>("radio_button");
	this->context->inflater.register_widget<choice_group>("choice_group");
	this->context->inflater.register_widget<MouseCursor>("mouse_cursor");
	this->context->inflater.register_widget<CollapseArea>("collapse_area");
	this->context->inflater.register_widget<image_toggle>("image_toggle");
	this->context->inflater.register_widget<image_push_button>("image_push_button");
	this->context->inflater.register_widget<Tabs>("tabs");
	this->context->inflater.register_widget<Tab>("tab");
	this->context->inflater.register_widget<TextInputLine>("text_input_line");

	try{
		auto t = this->context->loader.load<res_puu>("morda_gui_defs");

		this->context->inflater.inflate(t->forest());

	}catch(std::exception&){
		//TODO: do not ignore?
		TRACE(<< "gui::initStandardWidgets(): could not load morda_gui_definitions" << std::endl)
	}
}

void gui::set_viewport(const morda::Vec2r& size){
	this->viewportSize = size;

	if(!this->rootWidget){
		return;
	}

	this->rootWidget->resize(this->viewportSize);
}

void gui::set_root(std::shared_ptr<morda::widget> w){
	this->rootWidget = std::move(w);

	this->rootWidget->move_to(morda::Vec2r(0));
	this->rootWidget->resize(this->viewportSize);
}

void gui::render(const Matr4r& matrix)const{
	if(!this->rootWidget){
		TRACE(<< "gui::render(): root widget is not set" << std::endl)
		return;
	}

	morda::Matr4r m(matrix);

	// direct y-axis down
	m.scale(1, -1);

	m.translate(-1, -1);
	m.scale(Vec2r(2).compDivBy(this->viewportSize));

	ASSERT(this->rootWidget)

	if(this->rootWidget->is_layout_invalid()){
		TRACE(<< "root widget re-layout needed!" << std::endl)
		this->rootWidget->relayoutNeeded = false;
		this->rootWidget->lay_out();
	}

	this->rootWidget->renderInternal(m);
}

void gui::send_mouse_move(const Vec2r& pos, unsigned id){
	if(!this->rootWidget){
		return;
	}

	if(this->rootWidget->is_interactive()){
		this->rootWidget->set_hovered(this->rootWidget->rect().overlaps(pos), id);
		this->rootWidget->on_mouse_move(pos, id);
	}
}

void gui::send_mouse_button(bool is_down, const Vec2r& pos, mouse_button button, unsigned id){
	if(!this->rootWidget){
		return;
	}

	if(this->rootWidget->is_interactive()){
		this->rootWidget->set_hovered(this->rootWidget->rect().overlaps(pos), id);
		this->rootWidget->on_mouse_button(is_down, pos, button, id);
	}
}

void gui::send_mouse_hover(bool isHovered, unsigned pointerID){
	if(!this->rootWidget){
		return;
	}

	this->rootWidget->set_hovered(isHovered, pointerID);
}

void gui::send_key(bool is_down, key key_code){
//		TRACE(<< "HandleKeyEvent(): is_down = " << is_down << " is_char_input_only = " << is_char_input_only << " keyCode = " << unsigned(keyCode) << std::endl)

	if(auto w = this->context->focused_widget.lock()){
//		TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->onKeyInternal(is_down, key_code);
	}else{
//		TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		if(this->rootWidget){
			this->rootWidget->onKeyInternal(is_down, key_code);
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
