#include "gui.hpp"

#include "res/res_puu.hpp"

#include "widgets/slider/ScrollBar.hpp"

#include "widgets/button/NinePatchPushButton.hpp"
#include "widgets/button/CheckBox.hpp"
#include "widgets/button/RadioButton.hpp"
#include "widgets/button/ChoiceGroup.hpp"
#include "widgets/button/ImageToggle.hpp"
#include "widgets/button/ImagePushButton.hpp"
#include "widgets/button/Tab.hpp"
#include "widgets/button/Tabs.hpp"
#include "widgets/button/DropDownSelector.hpp"

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
		std::shared_ptr<morda::Renderer> r,
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
			this->context->loader.mountResPack(fi);
		}catch(std::runtime_error&){
//			TRACE(<< "could not mount " << s << std::endl)
			continue;
		}
		mounted = true;
		break;
	}

	if(!mounted){
		throw morda::Exc("gui::initStandardWidgets(): could not mount default resource pack");
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
	this->context->inflater.register_widget<CheckBox>("check_box");
	this->context->inflater.register_widget<TreeView>("tree_view");
	this->context->inflater.register_widget<DropDownSelector>("drop_down_selector");
	this->context->inflater.register_widget<RadioButton>("radio_button");
	this->context->inflater.register_widget<ChoiceGroup>("choice_group");
	this->context->inflater.register_widget<MouseCursor>("mouse_cursor");
	this->context->inflater.register_widget<CollapseArea>("collapse_area");
	this->context->inflater.register_widget<ImageToggle>("image_toggle");
	this->context->inflater.register_widget<ImagePushButton>("image_push_button");
	this->context->inflater.register_widget<Tabs>("tabs");
	this->context->inflater.register_widget<Tab>("tab");
	this->context->inflater.register_widget<TextInputLine>("text_input_line");

	try{
		auto t = this->context->loader.load<res_puu>("morda_gui_defs");

		this->context->inflater.inflate(t->forest());

	}catch(resource_loader::Exc&){
		// ignore
		TRACE(<< "gui::initStandardWidgets(): could not load morda_gui_definitions" << std::endl)
	}
}

void gui::setViewportSize(const morda::Vec2r& size){
	this->viewportSize = size;

	if(!this->rootWidget){
		return;
	}

	this->rootWidget->resize(this->viewportSize);
}

void gui::setRootWidget(const std::shared_ptr<morda::Widget> w){
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

void gui::onMouseMove(const Vec2r& pos, unsigned id){
	if(!this->rootWidget){
		return;
	}

	if(this->rootWidget->is_interactive()){
		this->rootWidget->set_hovered(this->rootWidget->rect().overlaps(pos), id);
		this->rootWidget->on_mouse_move(pos, id);
	}
}

void gui::onMouseButton(bool isDown, const Vec2r& pos, MouseButton_e button, unsigned pointerID){
	if(!this->rootWidget){
		return;
	}

	if(this->rootWidget->is_interactive()){
		this->rootWidget->set_hovered(this->rootWidget->rect().overlaps(pos), pointerID);
		this->rootWidget->on_mouse_button(isDown, pos, button, pointerID);
	}
}

void gui::onMouseHover(bool isHovered, unsigned pointerID){
	if(!this->rootWidget){
		return;
	}

	this->rootWidget->set_hovered(isHovered, pointerID);
}

void gui::onKeyEvent(bool isDown, key keyCode){
//		TRACE(<< "HandleKeyEvent(): is_down = " << is_down << " is_char_input_only = " << is_char_input_only << " keyCode = " << unsigned(keyCode) << std::endl)

	if(auto w = this->context->focused_widget.lock()){
//		TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->onKeyInternal(isDown, keyCode);
	}else{
//		TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		if(this->rootWidget){
			this->rootWidget->onKeyInternal(isDown, keyCode);
		}
	}
}

void gui::onCharacterInput(const UnicodeProvider& unicode, key key){
	if(auto w = this->context->focused_widget.lock()){
		//			TRACE(<< "HandleCharacterInput(): there is a focused widget" << std::endl)
		if(auto c = dynamic_cast<CharInputWidget*>(w.operator->())){
			c->onCharacterInput(unicode.get(), key);
		}
	}
}

void gui::postToUiThread(std::function<void()>&& f){
	this->context->run_from_ui_thread(std::move(f));
}
