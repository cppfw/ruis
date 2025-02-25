/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "layout/linear_layout.hpp"
#include "layout/pile_layout.hpp"
#include "layout/size_layout.hpp"
#include "layout/trivial_layout.hpp"
#include "res/tml.hpp"
#include "widget/button/choice_group.hpp"
#include "widget/button/impl/check_box.hpp"
#include "widget/button/impl/drop_down_box.hpp"
#include "widget/button/impl/image_push_button.hpp"
#include "widget/button/impl/image_toggle.hpp"
#include "widget/button/impl/nine_patch_push_button.hpp"
#include "widget/button/impl/radio_button.hpp"
#include "widget/button/tab.hpp"
#include "widget/button/tab_group.hpp"
#include "widget/group/book.hpp"
#include "widget/group/collapse_area.hpp"
#include "widget/group/overlay.hpp"
#include "widget/group/tabbed_book.hpp"
#include "widget/group/tree_view.hpp"
#include "widget/group/window.hpp"
#include "widget/input/text_input_line.hpp"
#include "widget/label/busy.hpp"
#include "widget/label/color.hpp"
#include "widget/label/gradient.hpp"
#include "widget/label/image_mouse_cursor.hpp"
#include "widget/label/spinner.hpp"
#include "widget/label/text.hpp"
#include "widget/proxy/aspect_ratio_proxy.hpp"
#include "widget/proxy/click_proxy.hpp"
#include "widget/proxy/key_proxy.hpp"
#include "widget/proxy/min_proxy.hpp"
#include "widget/proxy/resize_proxy.hpp"
#include "widget/slider/scroll_bar.hpp"

using namespace ruis;

namespace {
const auto default_defs = R"qwertyuiop(
defs{
	@pile{
		@container{
			layout{pile}

			${children}
		}
	}

	@row{
		@container{
			layout{row}

			${children}
		}
	}

	@column{
		@container{
			layout{column}

			${children}
		}
	}
}

defs{
	@margins{ left top right bottom weight_left weight_right weight_top weight_bottom
		@row{
			@widget{
				lp{
					dx{${left}}
					weight{${weight_left}}
				}
			}
			@column{
				lp{
					weight{1} dx{max} dy{max}
				}
				@widget{
					lp{
						dy{${top}}
						weight{${weight_top}}
					}
				}
				@pile{
					lp{
						weight{1} dx{max} dy{max}
					}
					${children}
				}
				@widget{
					lp{
						dy{${bottom}}
						weight{${weight_bottom}}
					}
				}
			}
			@widget{
				lp{
					dx{${right}}
					weight{${weight_right}}
				}
			}
		}
	}

	@left{ lp
		@row{
			lp{
				${lp}
				dx{max}
			}

			${children}

			@widget{
				lp{
					dx{fill}
					weight{1}
				}
			}
		}
	}

	@right{ lp
		@row{
			lp{
				${lp}
				dx{max}
			}

			@widget{
				lp{
					dx{fill}
					weight{1}
				}
			}

			${children}
		}
	}

	@top{ lp
		@column{
			lp{
				${lp}
				dy{max}
			}

			${children}

			@widget{
				lp{
					dy{fill}
					weight{1}
				}
			}
		}
	}

	@bottom{ lp
		@column{
			lp{
				${lp}
				dy{max}
			}

			@widget{
				lp{
					dy{fill}
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
} // namespace

// NOLINTNEXTLINE(modernize-pass-by-value)
gui::gui(const utki::shared_ref<ruis::context>& context) :
	context(context),
	root_widget(utki::make_shared<ruis::widget>(this->context, tml::forest()))
{
	// register basic widgets
	this->context.get().inflater.register_widget<widget>("widget");
	this->context.get().inflater.register_widget<container>("container");

	// proxy
	this->context.get().inflater.register_widget<mouse_proxy>("mouse_proxy");
	this->context.get().inflater.register_widget<click_proxy>("click_proxy");
	this->context.get().inflater.register_widget<aspect_ratio_proxy>("aspect_ratio_proxy");
	this->context.get().inflater.register_widget<key_proxy>("key_proxy");
	this->context.get().inflater.register_widget<min_proxy>("min_proxy");

	this->context.get().inflater.register_widget<scroll_area>("scroll_area");
	this->context.get().inflater.register_widget<overlay>("overlay");
	this->context.get().inflater.register_widget<pan_list>("pan_list");
	this->context.get().inflater.register_widget<list>("list");
	this->context.get().inflater.register_widget<book>("book");

	// label
	this->context.get().inflater.register_widget<text>("text");
	this->context.get().inflater.register_widget<color>("color");
	this->context.get().inflater.register_widget<gradient>("gradient");
	this->context.get().inflater.register_widget<image>("image");
	this->context.get().inflater.register_widget<nine_patch>("nine_patch");
	this->context.get().inflater.register_widget<spinner>("spinner");

	// button
	this->context.get().inflater.register_widget<nine_patch_push_button>("nine_patch_push_button");

	this->context.get().inflater.push_defs(default_defs);

	// register basic layouts
	this->context.get().layout_factory.add_factory("trivial", [](const auto&) {
		return layout::trivial;
	});
	this->context.get().layout_factory.add_factory("size", [](const auto&) {
		return layout::size;
	});
	this->context.get().layout_factory.add_factory("pile", [](const auto&) {
		return layout::pile;
	});
	this->context.get().layout_factory.add_factory("row", [](const auto&) {
		return layout::row;
	});
	this->context.get().layout_factory.add_factory("column", [](const auto&) {
		return layout::column;
	});
}

void gui::init_standard_widgets(papki::file& fi)
{
	// mount default resource pack

	std::vector<std::string> paths;

	if (fi.path().length() != 0) {
		paths.push_back(fi.path());
	}

	paths.emplace_back("ruis_res/");

#if (M_OS == M_OS_LINUX && M_OS_NAME != M_OS_NAME_ANDROID) || (M_OS == M_OS_MACOSX && M_OS_NAME != M_OS_NAME_IOS) || \
	(M_OS == M_OS_UNIX)

	unsigned soname =
#	include "../soname.txt"
		;

	{
		std::stringstream ss;
		ss << "/usr/local/share/ruis/res" << soname << "/";
		paths.push_back(ss.str());
	}
	{
		std::stringstream ss;
		ss << "/usr/share/ruis/res" << soname << "/";
		paths.push_back(ss.str());
	}
#endif

	bool mounted = false;
	for (const auto& s : paths) {
		try {
			//			TRACE(<< "s = " << s << std::endl)
			fi.set_path(s);
			//			TRACE(<< "fi.path() = " << fi.path() << std::endl)
			this->context.get().loader.mount_res_pack(fi);
		} catch (std::runtime_error&) {
			//			TRACE(<< "could not mount " << s << std::endl)
			continue;
		}
		mounted = true;
		break;
	}

	if (!mounted) {
		throw std::runtime_error("gui::init_standard_widgets(): could not mount default resource pack");
	}

	// add standard widgets to inflater
	this->context.get().inflater.register_widget<window>("window");

	this->context.get().inflater.register_widget<check_box>("check_box");
	this->context.get().inflater.register_widget<tree_view>("tree_view");
	this->context.get().inflater.register_widget<drop_down_box>("drop_down_box");
	this->context.get().inflater.register_widget<radio_button>("radio_button");
	this->context.get().inflater.register_widget<choice_group>("choice_group");
	this->context.get().inflater.register_widget<image_mouse_cursor>("image_mouse_cursor");
	this->context.get().inflater.register_widget<collapse_area>("collapse_area");
	this->context.get().inflater.register_widget<image_toggle>("image_toggle");
	this->context.get().inflater.register_widget<image_push_button>("image_push_button");
	this->context.get().inflater.register_widget<tab_group>("tab_group");
	this->context.get().inflater.register_widget<tab>("tab");
	this->context.get().inflater.register_widget<text_input_line>("text_input_line");
	this->context.get().inflater.register_widget<busy>("busy");
	this->context.get().inflater.register_widget<tabbed_book>("tabbed_book");

	auto t = this->context.get().loader.load<res::tml>("ruis_gui_defs");

	this->context.get().inflater.push_defs(t.get().forest());
}

void gui::set_viewport(const ruis::vector2& size)
{
	this->viewport_size = size;

	this->root_widget.get().resize(this->viewport_size);
}

void gui::set_root(utki::shared_ref<ruis::widget> w)
{
	if (w.get().parent()) {
		throw std::invalid_argument("given widget is already added to some container");
	}

	this->root_widget = std::move(w);

	this->root_widget.get().move_to(ruis::vector2(0));
	this->root_widget.get().resize(this->viewport_size);
}

void gui::render(const matrix4& matrix) const
{
	if (this->get_root().is_layout_dirty()) {
		LOG([](auto& o) {
			o << "root widget re-layout needed!" << std::endl;
		})
		// TODO: render() is const, but calls non-const lay_out(), fix it somehow? Perhaps make render() non-const?
		this->root_widget.get().lay_out();
	}

	ruis::matrix4 m = make_viewport_matrix(matrix, this->viewport_size);

	this->get_root().render_internal(m);
}

void gui::send_mouse_move(const vector2& pos, unsigned id)
{
	auto& rw = this->get_root();
	if (rw.is_interactive()) {
		rw.set_hovered(rw.rect().overlaps(pos), id);
		rw.on_mouse_move(mouse_move_event{pos, id, false});
	}
}

void gui::send_mouse_button(bool is_down, const vector2& pos, mouse_button button, unsigned id)
{
	auto& rw = this->get_root();
	if (rw.is_interactive()) {
		rw.set_hovered(rw.rect().overlaps(pos), id);
		rw.on_mouse_button(mouse_button_event{is_down, pos, button, id});
	}
}

void gui::send_mouse_hover(bool is_hovered, unsigned pointer_id)
{
	this->get_root().set_hovered(is_hovered, pointer_id);
}

void gui::send_key(bool is_down, key key_code)
{
	//		TRACE(<< "HandleKeyEvent(): is_down = " << is_down << " is_char_input_only = " << is_char_input_only << "
	// keyCode = " << unsigned(keyCode) << std::endl)

	auto modifier = ruis::to_key_modifier(key_code);
	if (modifier != ruis::key_modifier::unknown) {
		this->key_modifiers.set(modifier, is_down);
	}

	ruis::key_event e;
	e.combo.key = key_code;
	e.combo.modifiers = this->key_modifiers;
	e.is_down = is_down;

	if (auto w = this->context.get().focused_widget.lock()) {
		//		TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->on_key_internal(e);
	} else {
		//		TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		this->get_root().on_key_internal(e);
	}
}

void gui::send_character_input(const input_string_provider& string_provider, key key_code)
{
	if (auto w = this->context.get().focused_widget.lock()) {
		character_input_event e;
		auto str = string_provider.get();
		e.string = str;
		e.combo.key = key_code;
		e.combo.modifiers = this->key_modifiers;

		if (auto c = dynamic_cast<character_input_widget*>(w.get())) {
			c->on_character_input(e);
		}
	}
}
