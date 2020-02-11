#include "context.hpp"

#include <utki/config.hpp>

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


gui::T_Instance gui::instance;


gui::gui(
		std::shared_ptr<morda::Renderer> r,
		real dotsPerInch,
		real dotsPerDp,
		std::function<void(std::function<void()>&&)>&& postToUiThreadFunction
	) :
		renderer(std::move(r)),
		resMan(*this),
		postToUiThread_v(std::move(postToUiThreadFunction)),
		units(dotsPerInch, dotsPerDp)
{
	if(!this->renderer){
		throw std::invalid_argument("gui::gui(): passed in renderer pointer is null");
	}
	if(!this->postToUiThread_v){
		throw std::invalid_argument("gui::gui(): no post to UI thread function provided");
	}
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
			this->resMan.mountResPack(fi);
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

	this->inflater.register_widget<Text>("Text");
	this->inflater.register_widget<Color>("Color");
	this->inflater.register_widget<Gradient>("Gradient");
	this->inflater.register_widget<Image>("Image");
	this->inflater.register_widget<VScrollBar>("VScrollBar");
	this->inflater.register_widget<HScrollBar>("HScrollBar");
	this->inflater.register_widget<Window>("Window");
	this->inflater.register_widget<NinePatch>("NinePatch");
	this->inflater.register_widget<NinePatchPushButton>("NinePatchPushButton");
	this->inflater.register_widget<CheckBox>("CheckBox");
	this->inflater.register_widget<TreeView>("TreeView");
	this->inflater.register_widget<DropDownSelector>("DropDownSelector");
	this->inflater.register_widget<RadioButton>("RadioButton");
	this->inflater.register_widget<ChoiceGroup>("ChoiceGroup");
	this->inflater.register_widget<MouseCursor>("MouseCursor");
	this->inflater.register_widget<CollapseArea>("CollapseArea");
	this->inflater.register_widget<ImageToggle>("ImageToggle");
	this->inflater.register_widget<ImagePushButton>("ImagePushButton");
	this->inflater.register_widget<Tabs>("Tabs");
	this->inflater.register_widget<Tab>("Tab");
	this->inflater.register_widget<TextInputLine>("TextInputLine");

	try{
		auto t = morda::gui::inst().resMan.load<res_puu>("morda_gui_defs");

		this->inflater.inflate(t->forest());

	}catch(resource_loader::Exc&){
		//ignore
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

	//direct y axis down
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

	if(auto w = this->focusedWidget.lock()){
//		TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->onKeyInternal(isDown, keyCode);
	}else{
//		TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		if(this->rootWidget){
			this->rootWidget->onKeyInternal(isDown, keyCode);
		}
	}
}


void gui::setFocusedWidget(const std::shared_ptr<Widget> w){
	if(auto prev = this->focusedWidget.lock()){
		prev->isFocused_v = false;
		prev->on_focus_changed();
	}

	this->focusedWidget = w;

	if(w){
		w->isFocused_v = true;
		w->on_focus_changed();
	}
}

void gui::onCharacterInput(const UnicodeProvider& unicode, key key){
	if(auto w = this->focusedWidget.lock()){
		//			TRACE(<< "HandleCharacterInput(): there is a focused widget" << std::endl)
		if(auto c = dynamic_cast<CharInputWidget*>(w.operator->())){
			c->onCharacterInput(unicode.get(), key);
		}
	}
}

void gui::postToUiThread(std::function<void()>&& f) {
	ASSERT(this->postToUiThread_v)

	this->postToUiThread_v(std::move(f));
}
