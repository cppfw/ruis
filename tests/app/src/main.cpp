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
#include <ruis/widget/label/color.hpp>
#include <ruis/widget/label/image.hpp>
#include <ruis/widget/input/text_input_line.hpp>
#include <ruis/widget/button/selection_box.hpp>
#include <ruis/layout/linear_layout.hpp>

#include "window0.hpp"
#include "window1.hpp"
#include "tree_view_window.hpp"

#ifdef assert
#	undef assert
#endif

using namespace ruis::length_literals;

class simple_widget :
		virtual public ruis::widget,
		public ruis::updateable,
		public ruis::character_input_widget
{
	utki::shared_ref<const ruis::res::texture_2d> tex;

public:
	simple_widget(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
			ruis::widget(c, desc),
			ruis::character_input_widget(this->context),
			tex(this->context.get().loader.load<ruis::res::texture_2d>("tex_sample"))
	{}

	uint32_t timer = 0;
	uint32_t cnt = 0;

	void update(uint32_t dt) override{
		this->timer += dt;
		++this->cnt;

		if(this->timer > std::milli::den){
			this->timer -= std::milli::den;

			LOG([this](auto&o){o << "Update(): UPS = " << this->cnt << std::endl;})

			this->cnt = 0;
		}
	}

	bool on_mouse_button(const ruis::mouse_button_event& e)override{
		LOG([&](auto&o){o << "OnMouseButton(): isDown = " << e.is_down << ", pos = " << e.pos << ", button = " << unsigned(e.button) << ", pointer_id = " << e.pointer_id << std::endl;})
		if(!e.is_down){
			return false;
		}

		if(this->is_updating()){
			this->context.get().updater.get().stop(*this);
		}else{
			this->context.get().updater.get().start(
					utki::make_shared_from(*this), //
					0
				);
		}
		this->focus();
		return true;
	}

	bool on_key(const ruis::key_event& e)override{
		if(e.is_down){
			LOG([&](auto&o){o << "simple_widget::OnKey(): down, keyCode = " << unsigned(e.combo.key) << std::endl;})
			switch(e.combo.key){
				case ruis::key::arrow_left:
					utki::log([](auto&o){o << "simple_widget::OnKeyDown(): LEFT key caught" << std::endl;});
					return true;
				case ruis::key::a:
					utki::log([](auto&o){o << "simple_widget::OnKeyUp(): A key caught" << std::endl;});
					return true;
				default:
					break;
			}
		}else{
			LOG([&](auto&o){o << "simple_widget::OnKey(): up, keyCode = " << unsigned(e.combo.key) << std::endl;})
			switch(e.combo.key){
				case ruis::key::arrow_left:
					utki::log([](auto&o){o << "simple_widget::OnKeyUp(): LEFT key caught" << std::endl;});
					return true;
				case ruis::key::a:
					utki::log([](auto&o){o << "simple_widget::OnKeyUp(): A key caught" << std::endl;});
					return true;
				default:
					break;
			}
		}
		return false;
	}

	void on_character_input(const ruis::character_input_event& e)override{
		if(e.string.empty()){
			return;
		}

		LOG([&](auto&o){o << "simple_widget::on_character_input(): string = " << uint32_t(e.string[0]) << std::endl;})
	}

	void render(const ruis::matrix4& matrix)const override{
		{
			ruis::matrix4 matr(matrix);
			matr.scale(this->rect().d);

			auto& r = this->context.get().renderer.get();

			r.shader->pos_tex->render(matr, r.pos_tex_quad_01_vao.get(), this->tex.get().tex());
		}

//		this->fnt->Fnt().RenderTex(s , matrix);

//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glEnable(GL_BLEND);
//		ruis::SimpleTexturingShader &s = ruis::gui::inst().shaders.simpleTexturing;
//		ruis::matrix4 m(matrix);
//		m.translate(200, 200);
//		this->fnt->Fnt().RenderString(s, m, "Hello World!");
	}
};

class cube_widget : public ruis::widget, public ruis::updateable{
	std::shared_ptr<ruis::res::texture_2d> tex;

	ruis::quaternion rot = ruis::quaternion().set_identity();
public:
	std::shared_ptr<ruis::render::vertex_array> cubeVAO;

	cube_widget(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
			ruis::widget(c, desc)
	{
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
		std::array<ruis::vector3, 36> cube_pos = {{
			ruis::vector3(-1, -1,  1), ruis::vector3( 1, -1,  1), ruis::vector3(-1,  1,  1),
 			ruis::vector3( 1, -1,  1), ruis::vector3( 1,  1,  1), ruis::vector3(-1,  1,  1),
			ruis::vector3( 1, -1,  1), ruis::vector3( 1, -1, -1), ruis::vector3( 1,  1,  1),
			ruis::vector3( 1, -1, -1), ruis::vector3( 1,  1, -1), ruis::vector3( 1,  1,  1),
			ruis::vector3( 1, -1, -1), ruis::vector3(-1, -1, -1), ruis::vector3( 1,  1, -1),
			ruis::vector3(-1, -1, -1), ruis::vector3(-1,  1, -1), ruis::vector3( 1,  1, -1),
			ruis::vector3(-1, -1, -1), ruis::vector3(-1, -1,  1), ruis::vector3(-1,  1, -1),
			ruis::vector3(-1, -1,  1), ruis::vector3(-1,  1,  1), ruis::vector3(-1,  1, -1),
			ruis::vector3(-1,  1, -1), ruis::vector3(-1,  1,  1), ruis::vector3( 1,  1, -1),
			ruis::vector3(-1,  1,  1), ruis::vector3( 1,  1,  1), ruis::vector3( 1,  1, -1),
			ruis::vector3(-1, -1, -1), ruis::vector3( 1, -1, -1), ruis::vector3(-1, -1,  1),
			ruis::vector3(-1, -1,  1), ruis::vector3( 1, -1, -1), ruis::vector3( 1, -1,  1)
		}};

		auto pos_vbo = this->context.get().renderer.get().factory->create_vertex_buffer(utki::make_span(cube_pos));

		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
		std::array<ruis::vector2, 36> cube_tex = {{
			ruis::vector2(0, 0), ruis::vector2(1, 0), ruis::vector2(0, 1),
			ruis::vector2(1, 0), ruis::vector2(1, 1), ruis::vector2(0, 1),
			ruis::vector2(0, 0), ruis::vector2(1, 0), ruis::vector2(0, 1),
			ruis::vector2(1, 0), ruis::vector2(1, 1), ruis::vector2(0, 1),
			ruis::vector2(0, 0), ruis::vector2(1, 0), ruis::vector2(0, 1),
			ruis::vector2(1, 0), ruis::vector2(1, 1), ruis::vector2(0, 1),
			ruis::vector2(0, 0), ruis::vector2(1, 0), ruis::vector2(0, 1),
			ruis::vector2(1, 0), ruis::vector2(1, 1), ruis::vector2(0, 1),
			ruis::vector2(0, 0), ruis::vector2(1, 0), ruis::vector2(0, 1),
			ruis::vector2(1, 0), ruis::vector2(1, 1), ruis::vector2(0, 1),
			ruis::vector2(0, 0), ruis::vector2(1, 0), ruis::vector2(0, 1),
			ruis::vector2(1, 0), ruis::vector2(1, 1), ruis::vector2(0, 1)
		}};

		auto tex_vbo = this->context.get().renderer.get().factory->create_vertex_buffer(utki::make_span(cube_tex));

		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
		std::array<uint16_t, 36> indices = {{
			// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
		}};

		auto cube_indices = this->context.get().renderer.get().factory->create_index_buffer(utki::make_span(indices));

		this->cubeVAO = this->context.get().renderer.get().factory->create_vertex_array(
			{pos_vbo, tex_vbo},
			cube_indices,
			ruis::render::vertex_array::mode::triangles
		).to_shared_ptr();

		this->tex = this->context.get().loader.load<ruis::res::texture_2d>("tex_sample").to_shared_ptr();
		this->rot.set_identity();
	}

	unsigned fps = 0;
	uint32_t fpsSecCounter = 0;

	void update(uint32_t dt) override{
		this->fpsSecCounter += dt;
		++this->fps;
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
		this->rot *= ruis::quaternion().set_rotation(r4::vector3<float>(1, 2, 1).normalize(), 1.5f * (float(dt) / std::milli::den));
		if(this->fpsSecCounter >= std::milli::den){
			std::cout << "fps = " << std::dec << fps << std::endl;
			this->fpsSecCounter = 0;
			this->fps = 0;
		}
		this->clear_cache();
	}

	void render(const ruis::matrix4& matrix)const override{		
		ruis::matrix4 matr(matrix);
		matr.scale(this->rect().d / 2);
		matr.translate(1, 1);
		matr.scale(1, -1, -1); // y down, z away
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
		matr.frustum(-2, 2, -1.5, 1.5, 2, 100);
		matr.translate(0, 0, -4);
		matr.rotate(this->rot);

		// std::cout << "depth = " << this->get_params().depth << std::endl;

		// glDisable(GL_CULL_FACE);

		this->context.get().renderer.get().shader->pos_tex->render(matr, *this->cubeVAO, this->tex->tex());
	}
};

class tree_view_items_provider : public ruis::tree_view::provider{
	tml::forest root;

	utki::shared_ref<ruis::context> context;
public:

	// NOLINTNEXTLINE(modernize-pass-by-value)
	tree_view_items_provider(const utki::shared_ref<ruis::context>& c) :
			provider(c),
			context(c)
	{
		this->root = tml::read(R"qwertyuiop(
				root1{
					subroot1{
						subsubroot1
						subsubroot2
						subsubroot3
						subsubroot4
					}
					subroot2
					subroot3{
						subsubroot0
						subsubroot1{
							subsubsubroot1
							subsubsubroot2
						}
						subsubroot2
					}
				}
				root22{
					subsubroot1
					subsubroot2{
						trololo
						"hello world!"
					}
				}
				root333
				root4444
			)qwertyuiop");
	}

	tree_view_items_provider(const tree_view_items_provider&) = delete;
	tree_view_items_provider& operator=(const tree_view_items_provider&) = delete;

	tree_view_items_provider(tree_view_items_provider&&) = delete;
	tree_view_items_provider& operator=(tree_view_items_provider&&) = delete;

	~tree_view_items_provider()override = default;

private:
	std::vector<size_t> selected_item;

	unsigned newItemNumber = 0;

	std::string generate_new_item_value(){
		std::stringstream ss;
		ss << "newItem" << newItemNumber;
		++newItemNumber;
		return ss.str();
	}

public:
	void insert_before(){
		if(this->selected_item.size() == 0){
			return;
		}

		tml::forest* list = &this->root;
		tml::forest* parent_list = nullptr;

		for(auto& i : this->selected_item){
			parent_list = list;
			list = &(*list)[i].children;
		}

		if(!parent_list){
			return;
		}

		parent_list->insert(utki::next(parent_list->begin(), this->selected_item.back()), tml::leaf(this->generate_new_item_value()));

		this->notify_item_add(utki::make_span(this->selected_item));
		++this->selected_item.back();
	}

	void insert_after(){
		if(this->selected_item.size() == 0){
			return;
		}

		tml::forest* list = &this->root;
		tml::forest* parent_list = nullptr;

		for(auto& i : this->selected_item){
			parent_list = list;
			list = &(*list)[i].children;
		}

		if(!parent_list){
			return;
		}

		parent_list->insert(utki::next(parent_list->begin(), this->selected_item.back() + 1), tml::leaf(this->generate_new_item_value()));

		++this->selected_item.back();
		this->notify_item_add(utki::make_span(this->selected_item));
		--this->selected_item.back();
	}

	void insert_child(){
		if(this->selected_item.size() == 0){
			return;
		}

		tml::forest* list = &this->root;

		for(auto& i : this->selected_item){
			list = &(*list)[i].children;
		}

		list->emplace_back(this->generate_new_item_value());

		this->selected_item.push_back(list->size() - 1);
		this->notify_item_add(utki::make_span(this->selected_item));
		this->selected_item.pop_back();
	}

	utki::shared_ref<ruis::widget> get_widget(utki::span<const size_t> path, bool is_collapsed)override{
		ASSERT(!path.empty())

		auto list = &this->root;
		decltype(list) parent_list = nullptr;

		tml::tree* n = nullptr;

		for(const auto& i : path){
			n = &(*list)[i];
			parent_list = list;
			list = &n->children;
		}

		auto ret = utki::make_shared<ruis::container>(this->context, tml::forest(), ruis::layout::row);

		{
			auto v = this->context.get().inflater.inflate(
					R"qwertyuiop(
							@pile{
								@color{
									id{selection}
									lp{dx{fill}dy{fill}}
									color{${ruis_color_highlight}}
									visible{false}
								}
								@text{
									id{value}
								}
								@mouse_proxy{
									id{mouse_proxy}
									lp{dx{fill}dy{fill}}
								}
							}
						)qwertyuiop"
				);

			{
				auto value = v.get().try_get_widget_as<ruis::text>("value");
				ASSERT(value)
				value->set_text(
						n->value.string // NOLINT(clang-analyzer-core.CallAndMessage): due to ASSERT(!path.empty()) in the beginning of the function 'n' is not nullptr
					);
			}
			{
				auto color_label = v.get().try_get_widget_as<ruis::color>("selection");

				color_label->set_visible(utki::deep_equals(path, utki::make_span(this->selected_item)));

				auto mp = v.get().try_get_widget_as<ruis::mouse_proxy>("mouse_proxy");
				ASSERT(mp)
				mp->mouse_button_handler = [this, path = utki::make_vector(path)](ruis::mouse_proxy&, const ruis::mouse_button_event& e) -> bool{
					if(!e.is_down || e.button != ruis::mouse_button::left){
						return false;
					}

					this->selected_item = path;
#ifdef DEBUG
					LOG([](auto&o){o << " selected item = ";})
					for(const auto& k : this->selected_item){
						LOG([&](auto&o){o << k << ", ";})
					}
					LOG([](auto&o){o << std::endl;})
#endif
					this->notify_item_change();

					return true;
				};
			}

			ret.get().push_back(v);
		}

		{
			auto b = this->context.get().inflater.inflate_as<ruis::push_button>(
					R"qwertyuiop(
							@push_button{
								@color{
									color{0xff0000ff}
									lp{dx{2mm}dy{0.5mm}}
								}
							}
						)qwertyuiop"
				);
			b.get().click_handler = [this, path = utki::make_vector(path), parent_list](ruis::push_button& button){
				ASSERT(parent_list)
				parent_list->erase(utki::next(parent_list->begin(), path.back()));
				this->notify_item_remove(utki::make_span(path));
			};
			ret.get().push_back(b);
		}

		return ret;
	}

	size_t count(utki::span<const size_t> path) const noexcept override{
		auto children = &this->root;

		for(auto& i : path){
			children = &(*children)[i].children;
		}

		return children->size();
	}

};

class application : public ruisapp::application{
public:
	application() :
			ruisapp::application("ruis-tests", [](){
				// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
				ruisapp::window_params wp(r4::vector2<unsigned>(1024, 800));
				wp.buffers.set(ruisapp::window_params::buffer::depth);

		return wp;
			}())
	{
		this->gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		this->gui.context.get().loader.mount_res_pack(*this->get_res_file("res/"));
//		this->ResMan().MountResPack(ruis::ZipFile::New(papki::FSFile::New("res.zip")));

		this->gui.context.get().inflater.register_widget<simple_widget>("U_SimpleWidget");
		this->gui.context.get().inflater.register_widget<cube_widget>("cube_widget");

		auto c = this->gui.context.get().inflater.inflate(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);

		c.get().get_widget("window0").replace_by(
			make_window0(
				this->gui.context, //
				{0, 0}
			)
		);

		c.get().get_widget("window1").replace_by(
			make_window1(
				this->gui.context, //
				{300_pp, 10_pp}
			)
		);

		c.get().get_widget("window2").replace_by(
			make_tree_view_window(
				this->gui.context, //
				{300_pp, 250_pp}
			)
		);

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
		std::dynamic_pointer_cast<ruis::push_button>(c.get().try_get_widget("show_VK_button"))->click_handler = [this](ruis::push_button&){
			this->show_virtual_keyboard();
		};

		std::dynamic_pointer_cast<ruis::push_button>(c.get().try_get_widget("push_button_in_scroll_container"))->click_handler = [this](ruis::push_button&){
			this->gui.context.get().post_to_ui_thread(
					[](){
						std::cout << "Print from UI thread!!!!!!!!" << std::endl;
					}
				);
		};

		// cube click_proxy
		{
			auto& cube = c.get().get_widget_as<cube_widget>("cube_widget");

			auto& cp = c.get().get_widget_as<ruis::click_proxy>("cube_click_proxy");
			auto& bg = c.get().get_widget_as<ruis::color>("cube_bg_color");
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

			auto vertical_slider = c.get().try_get_widget_as<ruis::vertical_scroll_bar>("vertical_list_slider");
			auto vs = utki::make_weak(vertical_slider);

			vertical_slider->fraction_change_handler = [vl](ruis::fraction_widget& slider){
				if(auto l = vl.lock()){
					l->set_scroll_factor(slider.get_fraction());
				}
			};

			vertical_list->scroll_change_handler = [vs](ruis::list& l){
				if(auto s = vs.lock()){
					s->set_fraction(l.get_scroll_factor(), false);
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
					h->set_fraction(l.get_scroll_factor(), false);
					h->set_band_fraction(l.get_scroll_band());
				}
			};

			horizontal_slider->fraction_change_handler = [hl](ruis::fraction_widget& slider){
//				TRACE(<< "horizontal slider factor = " << slider.factor() << std::endl)
				if(auto l = hl.lock()){
					l->set_scroll_factor(slider.get_fraction());
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

		// tree_view
		{
			auto treeview = ruis::make::tree_view(this->gui.context, {
				.widget_params = {
					.clip = true
				}
			});
			
			c.get().get_widget_as<ruis::tree_view>("treeview_widget").replace_by(treeview);

			auto provider = std::make_shared<tree_view_items_provider>(c.get().context);
			treeview.get().set_provider(provider);
			auto tv = utki::make_weak(treeview);

			auto& vertical_slider = c.get().get_widget_as<ruis::fraction_band_widget>("treeview_vertical_slider");
			auto vs = utki::make_weak_from(vertical_slider);

			vertical_slider.fraction_change_handler = [tv](ruis::fraction_widget& slider){
				if(auto t = tv.lock()){
					t->set_vertical_scroll_factor(slider.get_fraction());
				}
			};

			auto& horizontal_slider = c.get().get_widget_as<ruis::fraction_band_widget>("treeview_horizontal_slider");
			auto hs = utki::make_weak_from(horizontal_slider);

			horizontal_slider.fraction_change_handler = [tv](ruis::fraction_widget& slider){
				if(auto t = tv.lock()){
					t->set_horizontal_scroll_factor(slider.get_fraction());
				}
			};

			treeview.get().scroll_change_handler = [
					hs = utki::make_weak_from(horizontal_slider),
					vs = utki::make_weak_from(vertical_slider)
				](ruis::tree_view& tw)
			{
				auto f = tw.get_scroll_factor();
                auto b = tw.get_scroll_band();
				if(auto h = hs.lock()){
                    h->set_band_fraction(b.x());
					h->set_fraction(f.x(), false);
				}
				if(auto v = vs.lock()){
                    v->set_band_fraction(b.y());
					v->set_fraction(f.y(), false);
				}
			};

			auto insert_before_button = c.get().try_get_widget_as<ruis::push_button>("insert_before");
			auto insert_after_button = c.get().try_get_widget_as<ruis::push_button>("insert_after");
			auto insert_child = c.get().try_get_widget_as<ruis::push_button>("insert_child");

			auto prvdr = utki::make_weak(provider);
			insert_before_button->click_handler = [prvdr](ruis::push_button& b){
				if(auto p = prvdr.lock()){
					p->insert_before();
				}
			};

			insert_after_button->click_handler = [prvdr](ruis::push_button& b){
				if(auto p = prvdr.lock()){
					p->insert_after();
				}
			};

			insert_child->click_handler = [prvdr](ruis::push_button& b){
				if(auto p = prvdr.lock()){
					p->insert_child();
				}
			};
		}

		// fullscreen
		{
			auto b = c.get().try_get_widget_as<ruis::push_button>("fullscreen_button");
			ASSERT(b)
			b->click_handler = [this](ruis::push_button&) {
				this->set_fullscreen(!this->is_fullscreen());
			};
		}
		{
			auto b = c.get().try_get_widget_as<ruis::push_button>("image_push_button");
			ASSERT(b)
			b->click_handler = [this](ruis::push_button&) {
				this->set_fullscreen(true);
			};
		}

		// mouse cursor
		{
			auto b = c.get().try_get_widget_as<ruis::push_button>("showhide_mousecursor_button");
			bool visible = true;
			this->set_mouse_cursor_visible(visible);
			b->click_handler = [visible, this](ruis::push_button&) mutable{
				visible = !visible;
				this->set_mouse_cursor_visible(visible);
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
