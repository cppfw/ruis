/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "gui.hpp"

#include "res/treeml.hpp"

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
#include "widgets/label/image_mouse_cursor.hpp"
#include "widgets/label/spinner.hpp"

#include "widgets/input/text_input_line.hpp"

#include "widgets/group/tree_view.hpp"
#include "widgets/group/window.hpp"
#include "widgets/group/collapse_area.hpp"
#include "widgets/group/size_container.hpp"
#include "widgets/group/row.hpp"
#include "widgets/group/overlay.hpp"
#include "widgets/group/book.hpp"
#include "widgets/group/tabbed_book.hpp"

#include "widgets/proxy/click_proxy.hpp"
#include "widgets/proxy/key_proxy.hpp"
#include "widgets/proxy/resize_proxy.hpp"
#include "widgets/proxy/ratio_proxy.hpp"
#include "widgets/proxy/min_proxy.hpp"

using namespace morda;

namespace{
const auto default_defs = R"qwertyuiop(
defs{
	@margins{ left top right bottom weight_left weight_right weight_top weight_bottom
		@row{
			@widget{
				layout{
					dx{${left}}
					weight{${weight_left}}
				}
			}
			@column{
				layout{
					weight{1} dy{max}
				}
				@widget{
					layout{
						dy{${top}}
						weight{${weight_top}}
					}
				}
				@pile{
					layout{
						weight{1} dx{max}
					}
					${children}
				}
				@widget{
					layout{
						dy{${bottom}}
						weight{${weight_bottom}}
					}
				}
			}
			@widget{
				layout{
					dx{${right}}
					weight{${weight_right}}
				}
			}
		}
	}

	@left{ layout
		@row{
			layout{
				${layout}
				dx{max}
			}

			${children}

			@widget{
				layout{
					dx{0}
					weight{1}
				}
			}
		}
	}

	@right{ layout
		@row{
			layout{
				${layout}
				dx{max}
			}

			@widget{
				layout{
					dx{0}
					weight{1}
				}
			}

			${children}
		}
	}

	@top{ layout
		@column{
			layout{
				${layout}
				dy{max}
			}

			${children}

			@widget{
				layout{
					dy{0}
					weight{1}
				}
			}
		}
	}

	@bottom{ layout
		@column{
			layout{
				${layout}
				dy{max}
			}

			@widget{
				layout{
					dy{0}
					weight{1}
				}
			}

			${children}
		}
	}
}

defs{
	@margin{ value
		@margins{
			left{${value}}
			top{${value}}
			right{${value}}
			bottom{${value}}
		}
	}
}

)qwertyuiop";
}

gui::gui(std::shared_ptr<morda::context> context) :
		context(std::move(context))
{
	ASSERT(this->context)

	// register basic widgets
	this->context->inflater.register_widget<widget>("widget");
	this->context->inflater.register_widget<container>("container");
	this->context->inflater.register_widget<size_container>("size_container");
	this->context->inflater.register_widget<row>("row");
	this->context->inflater.register_widget<column>("column");
	this->context->inflater.register_widget<pile>("pile");
	this->context->inflater.register_widget<mouse_proxy>("mouse_proxy");
	this->context->inflater.register_widget<click_proxy>("click_proxy");
	this->context->inflater.register_widget<ratio_proxy>("ratio_proxy");
	this->context->inflater.register_widget<key_proxy>("key_proxy");
	this->context->inflater.register_widget<resize_proxy>("resize_proxy");
	this->context->inflater.register_widget<min_proxy>("min_proxy");

	this->context->inflater.register_widget<scroll_area>("scroll_area");
	this->context->inflater.register_widget<overlay>("overlay");
	this->context->inflater.register_widget<pan_list>("pan_list");
	this->context->inflater.register_widget<list>("list");
	this->context->inflater.register_widget<book>("book");
	this->context->inflater.register_widget<spinner>("spinner");

	this->context->inflater.inflate(default_defs);
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
	this->context->inflater.register_widget<image_mouse_cursor>("image_mouse_cursor");
	this->context->inflater.register_widget<collapse_area>("collapse_area");
	this->context->inflater.register_widget<image_toggle>("image_toggle");
	this->context->inflater.register_widget<image_push_button>("image_push_button");
	this->context->inflater.register_widget<tab_group>("tab_group");
	this->context->inflater.register_widget<tab>("tab");
	this->context->inflater.register_widget<text_input_line>("text_input_line");
	this->context->inflater.register_widget<busy>("busy");
	this->context->inflater.register_widget<tabbed_book>("tabbed_book");

	try{
		auto t = this->context->loader.load<res::treeml>("morda_gui_defs");

		this->context->inflater.push_defs(t->forest());

	}catch(std::exception&){
		//TODO: do not ignore?
		LOG([](auto&o){o << "gui::initStandardWidgets(): could not load morda_gui_definitions" << std::endl;})
	}
}

void gui::set_viewport(const morda::vector2& size){
	this->viewport_size = size;

	if(!this->root_widget){
		return;
	}

	this->root_widget->resize(this->viewport_size);
}

void gui::set_root(std::shared_ptr<morda::widget> w){
	if(w->parent()){
		throw std::invalid_argument("given widget is already added to some container");
	}

	this->root_widget = std::move(w);

	this->root_widget->move_to(morda::vector2(0));
	
	this->root_widget->resize(this->viewport_size);
}

void gui::render(const matrix4& matrix)const{
	if(!this->root_widget){
		LOG([](auto&o){o << "gui::render(): root widget is not set" << std::endl;})
		return;
	}

	ASSERT(this->root_widget)

	if(this->root_widget->is_layout_dirty()){
		LOG([](auto&o){o << "root widget re-layout needed!" << std::endl;})
		this->root_widget->layout_dirty = false;
		this->root_widget->lay_out();
	}

	morda::matrix4 m(matrix);

	// direct y-axis down
	m.scale(1, -1);

	m.translate(-1, -1);
	m.scale(vector2(2).comp_divide(this->viewport_size));

	this->root_widget->render_internal(m);
}

void gui::send_mouse_move(const vector2& pos, unsigned id){
	if(!this->root_widget){
		return;
	}

	if(this->root_widget->is_interactive()){
		this->root_widget->set_hovered(this->root_widget->rect().overlaps(pos), id);
		this->root_widget->on_mouse_move(mouse_move_event{pos, id, false});
	}
}

void gui::send_mouse_button(bool is_down, const vector2& pos, mouse_button button, unsigned id){
	if(!this->root_widget){
		return;
	}

	if(this->root_widget->is_interactive()){
		this->root_widget->set_hovered(this->root_widget->rect().overlaps(pos), id);
		this->root_widget->on_mouse_button(mouse_button_event{is_down, pos, button, id});
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

	auto modifier = morda::to_key_modifier(key_code);
	if(modifier != morda::key_modifier::unknown){
		this->key_modifiers.set(modifier, is_down);
	}

	morda::key_event e;
	e.combo.key = key_code;
	e.combo.modifiers = this->key_modifiers;
	e.is_down = is_down;

	if(auto w = this->context->focused_widget.lock()){
//		TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->on_key_internal(e);
	}else{
//		TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		if(this->root_widget){
			this->root_widget->on_key_internal(e);
		}
	}
}

void gui::send_character_input(const input_string_provider& string_provider, key key_code){
	if(auto w = this->context->focused_widget.lock()){
		character_input_event e;
		e.string = string_provider.get();
		e.combo.key = key_code;
		e.combo.modifiers = this->key_modifiers;

		if(auto c = dynamic_cast<character_input_widget*>(w.get())){
			c->on_character_input(e);
		}
	}
}
