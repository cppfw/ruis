#include <ratio>

// #include <GL/gl.h>

#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <ruisapp/application.hpp>

#include <ruis/config.hpp>
#include <ruis/widget/widget.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/res/texture_2d.hpp>
#include <ruis/widget/input/character_input_widget.hpp>
#include <ruis/widget/group/scroll_area.hpp>
#include <ruis/widget/proxy/mouse_proxy.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/group/list.hpp>
#include <ruis/widget/group/tree_view.hpp>
#include <ruis/widget/proxy/resize_proxy.hpp>
#include <ruis/widget/proxy/click_proxy.hpp>
#include <ruis/widget/label/image.hpp>
#include <ruis/widget/input/text_input_line.hpp>
#include <ruis/widget/button/selection_box.hpp>
#include <ruis/layout/linear_layout.hpp>
#include <ruis/default_style.hpp>

#include "root_widget.hpp"
#include "cube_widget.hpp"

#include "window0.hpp"
#include "window1.hpp"
#include "tree_view_window.hpp"
#include "sliders_window.hpp"
#include "spinning_cube_window.hpp"
#include "text_input_window.hpp"
#include "scroll_area_window.hpp"
#include "gradient_window.hpp"
#include "vertical_list_window.hpp"

#ifdef assert
#	undef assert
#endif

using namespace std::string_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
}

class application : public ruisapp::application{
	ruisapp::window& window;
public:
	application() :
			ruisapp::application(
				{
					.name = "ruis-tests"s
				}
			),
			window(this->make_window({
							.dims = {1024, 800},
							.buffers = {ruisapp::buffer::depth}
						}))
	{
		this->window.gui.context.get().window().close_handler = [this](){
			this->quit();
		};

		auto& gui = this->window.gui;

		gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		gui.context.get().loader().mount_res_pack(*this->get_res_file("res/"));
//		this->ResMan().MountResPack(ruis::ZipFile::New(papki::FSFile::New("res.zip")));

		auto c = make_root_widget(gui.context);
		gui.set_root(c);

		utki::dynamic_reference_cast<ruis::key_proxy>(c).get().key_handler = [this](ruis::key_proxy&, const ruis::key_event& e) -> bool {
			if(e.is_down){
				if(e.combo.key == ruis::key::escape){
					this->quit();
				}
			}
			return false;
		};

//		ruis::ZipFile zf(papki::FSFile::New("res.zip"), "test.gui.stob");
//		std::shared_ptr<ruis::widget> c = ruis::gui::inst().inflater().Inflate(zf);

		ASSERT(c.get().try_get_widget_as<ruis::push_button>("show_VK_button"))
		std::dynamic_pointer_cast<ruis::push_button>(c.get().try_get_widget("show_VK_button"))->click_handler = [](ruis::push_button& b){
			b.context.get().window().set_virtual_keyboard_visible(true);
		};

		std::dynamic_pointer_cast<ruis::push_button>(c.get().try_get_widget("push_button_in_scroll_container"))->click_handler = [ctx = gui.context](ruis::push_button&){
			ctx.get().post_to_ui_thread(
					[](){
						utki::logcat("Print from UI thread!!!!!!!!", '\n');
					}
				);
		};

		// cube click_proxy
		{
			auto& cube = c.get().get_widget_as<cube_widget>("cube_widget");

			auto& cp = c.get().get_widget_as<ruis::click_proxy>("cube_click_proxy");
			auto& bg = c.get().get_widget_as<ruis::rectangle>("cube_bg_color");
			cp.pressed_change_handler = [bg{utki::make_shared_from(bg)}](ruis::click_proxy& w) -> bool {
				// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
				bg.get().set_color(w.is_pressed() ? 0xff808080 : 0x80808080);
				return true;
			};
			cp.pressed_change_handler(cp); // set initial color
			cp.click_handler = [cube = utki::make_shared_from(cube)](ruis::click_proxy&) -> bool {
				if(cube.get().is_updating()){
					cube.get().context.get().updater.get().stop(cube.get());
				}else{
					cube.get().context.get().updater.get().start(cube, 0);
				}
				return true;
			};
		}

		// scroll_area
		{
			auto scroll_area = c.get().try_get_widget_as<ruis::scroll_area>("scroll_area");
			auto sa = utki::make_weak(scroll_area);

			auto vert_slider = c.get().try_get_widget_as<ruis::scroll_bar>("scroll_area_vertical_slider");
			auto vs = utki::make_weak(vert_slider);

			auto hori_slider = c.get().try_get_widget_as<ruis::scroll_bar>("scroll_area_horizontal_slider");
			auto hs = utki::make_weak(hori_slider);

			scroll_area->scroll_change_handler = [hs = hs, vs = vs](ruis::scroll_area& sa){
				auto f = sa.get_scroll_factor();
				auto b = sa.get_visible_area_fraction();
				if(auto h = hs.lock()){
					h->set_fraction(f.x());
					h->set_band_fraction(b.x());
				}
				if(auto v = vs.lock()){
					v->set_fraction(f.y());
					v->set_band_fraction(b.y());
				}
			};

			vert_slider->fraction_change_handler = [sa](ruis::fraction_widget& slider){
				if(auto s = sa.lock()){
					auto sf = s->get_scroll_factor();
					sf.y() = slider.get_fraction();
					s->set_scroll_factor(sf);
				}
			};

			hori_slider->fraction_change_handler = [sa](ruis::fraction_widget& slider){
				if(auto s = sa.lock()){
					auto sf = s->get_scroll_factor();
					sf.x() = slider.get_fraction();
					s->set_scroll_factor(sf);
				}
			};
		}

		// vertical_list
		{
			auto vertical_list = c.get().try_get_widget_as<ruis::list>("list");
			auto vl = utki::make_weak(vertical_list);

			auto& vertical_slider = c.get().get_widget_as<ruis::fraction_band_widget>("vertical_list_slider");
			auto vs = utki::make_weak_from(vertical_slider);

			vertical_slider.fraction_change_handler = [vl](ruis::fraction_widget& slider){
				if(auto l = vl.lock()){
					l->set_scroll_factor(
						slider.get_fraction(), //
						false
					);
				}
			};

			vertical_list->scroll_change_handler = [vs](ruis::list& l){
				if(auto s = vs.lock()){
					s->set_fraction(
						l.get_scroll_factor(), //
						false
					);
                    s->set_band_fraction(l.get_scroll_band());
				}
			};

			auto mouse_proxy = c.get().try_get_widget_as<ruis::mouse_proxy>("list_mouseproxy");
			struct button_state : public utki::shared{
				ruis::vector2 old_pos = 0;
				bool is_left_button_pressed = false;
			};
			auto state = std::make_shared<button_state>();

			static const ruis::real wheel_delta = 10;

			mouse_proxy->mouse_button_handler = [state, vl](ruis::mouse_proxy&, const ruis::mouse_button_event& e){
				switch(e.button){
					case ruis::mouse_button::left:
						state->is_left_button_pressed = e.is_down;
						state->old_pos = e.pos;
						return true;
					case ruis::mouse_button::wheel_down:
						if(auto l = vl.lock()){
							l->scroll_by(wheel_delta);
						}
						break;
					case ruis::mouse_button::wheel_up:
						if(auto l = vl.lock()){
							l->scroll_by(-wheel_delta);
						}
						break;
					default:
						break;
				}
				return false;
			};

			mouse_proxy->mouse_move_handler = [vs, vl, state](ruis::mouse_proxy&, const ruis::mouse_move_event& e){
				if(state->is_left_button_pressed){
					auto dp = state->old_pos - e.pos;
					state->old_pos = e.pos;
					if(auto l = vl.lock()){
						l->scroll_by(dp.y());
					}
					return true;
				}
				return false;
			};
		}

		// pan_list
		{
			auto pan_list = c.get().try_get_widget_as<ruis::list>("pan_list");
			auto hl = utki::make_weak(pan_list);

			auto horizontal_slider = c.get().try_get_widget_as<ruis::scroll_bar>("horizontal_list_slider");
			ASSERT(horizontal_slider)
			auto hs = utki::make_weak(horizontal_slider);

			pan_list->scroll_change_handler = [hs](ruis::list& l){
				if(auto h = hs.lock()){
					h->set_fraction(
						l.get_scroll_factor(), //
						false // do not notify
					);
					h->set_band_fraction(l.get_scroll_band());
				}
			};

			horizontal_slider->fraction_change_handler = [hl](ruis::fraction_widget& slider){
//				TRACE(<< "horizontal slider factor = " << slider.factor() << std::endl)
				if(auto l = hl.lock()){
					l->set_scroll_factor(
						slider.get_fraction(), //
						false // do not notify
					);
				}
			};

			auto mouse_proxy = c.get().try_get_widget_as<ruis::mouse_proxy>("horizontal_list_mouseproxy");
			struct button_state : public utki::shared{
				ruis::vector2 old_pos = 0;
				bool is_left_button_pressed = false;
			};
			auto state = std::make_shared<button_state>();

			static const ruis::real wheel_delta = 10;

			mouse_proxy->mouse_button_handler = [state, hl](ruis::mouse_proxy&, const ruis::mouse_button_event& e){
				std::cout << "button = " << unsigned(e.button) << std::endl;
				switch(e.button){
					case ruis::mouse_button::left:
						state->is_left_button_pressed = e.is_down;
						state->old_pos = e.pos;
						return true;
					case ruis::mouse_button::wheel_left:
						std::cout << "wheel_left" << std::endl;
						if(e.is_down){
							if(auto l = hl.lock()){
								l->scroll_by(-wheel_delta);
							}
						}
						break;
					case ruis::mouse_button::wheel_right:
						std::cout << "wheel_right" << std::endl;
						if(e.is_down){
							if(auto l = hl.lock()){
								l->scroll_by(wheel_delta);
							}
						}
						break;
					default:
						break;
				}
				return false;
			};

			mouse_proxy->mouse_move_handler = [hl, hs, state](ruis::mouse_proxy& w, const ruis::mouse_move_event& e) -> bool {
				if(state->is_left_button_pressed){
					auto dp = state->old_pos - e.pos;
					state->old_pos = e.pos;
					if(auto l = hl.lock()){
						l->scroll_by(dp.x());
					}
					return true;
				}
				return false;
			};
		}

		// text_input
		{
			auto& l = c.get().get_widget("text_input").get_widget<ruis::text_input_line>();
			utki::assert(!l.get_text().empty(), SL);
		}

		// fullscreen
		{
			auto b = c.get().try_get_widget_as<ruis::push_button>("fullscreen_button");
			ASSERT(b)
			b->click_handler = [](ruis::push_button& b) {
				auto& w = b.context.get().window();
				w.set_fullscreen(!w.is_fullscreen());
			};
		}
		{
			auto b = c.get().try_get_widget_as<ruis::push_button>("image_push_button");
			ASSERT(b)
			b->click_handler = [](ruis::push_button& b) {
				b.context.get().window().set_fullscreen(true);
			};
		}

		// mouse cursor
		{
			auto& b = c.get().get_widget_as<ruis::push_button>("showhide_mousecursor_button");
			bool visible = true;
			b.context.get().window().set_mouse_cursor_visible(visible);
			b.click_handler = [visible](ruis::push_button& b) mutable{
				visible = !visible;
				b.context.get().window().set_mouse_cursor_visible(visible);
			};
		}

		// dropdown
		{
			auto& dds = c.get().get_widget_as<ruis::selection_box>("dropdownselector");
			auto ddst = c.get().try_get_widget_as<ruis::text>("dropdownselector_selection");
			auto ddstw = utki::make_weak(ddst);

			dds.selection_handler = [ddstw](ruis::selection_box& dds){
				if(auto t = ddstw.lock()){
					std::stringstream ss;
					ss << "index_" << dds.get_selection();

					t->set_text(ss.str());
				}
			};
		}
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args){
	return std::make_unique<::application>();
});
