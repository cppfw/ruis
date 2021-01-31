#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <mordavokne/application.hpp>

#include "../../../src/morda/morda/config.hpp"

#include "../../../src/morda/morda/widgets/widget.hpp"
#include "../../../src/morda/morda/widgets/container.hpp"
#include "../../../src/morda/morda/widgets/proxy/key_proxy.hpp"

#include "../../../src/morda/morda/widgets/button/push_button.hpp"
#include "../../../src/morda/morda/widgets/label/text.hpp"

#include "../../../src/morda/morda/res/texture.hpp"

#include "../../../src/morda/morda/widgets/character_input_widget.hpp"
#include "../../../src/morda/morda/widgets/group/scroll_area.hpp"
#include "../../../src/morda/morda/widgets/group/row.hpp"
#include "../../../src/morda/morda/widgets/proxy/mouse_proxy.hpp"
#include "../../../src/morda/morda/widgets/slider/scroll_bar.hpp"
#include "../../../src/morda/morda/widgets/group/list.hpp"
#include "../../../src/morda/morda/widgets/group/tree_view.hpp"
#include "../../../src/morda/morda/widgets/proxy/resize_proxy.hpp"
#include "../../../src/morda/morda/widgets/proxy/click_proxy.hpp"
#include "../../../src/morda/morda/widgets/label/color.hpp"
#include "../../../src/morda/morda/widgets/label/image.hpp"
#include "../../../src/morda/morda/widgets/input/text_input_line.hpp"

#include "../../../src/morda/morda/widgets/button/drop_down_box.hpp"

class SimpleWidget :
		virtual public morda::widget,
		public morda::updateable,
		public morda::character_input_widget
{
	std::shared_ptr<morda::res::texture> tex;

public:
	SimpleWidget(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			morda::widget(std::move(c), desc),
			morda::character_input_widget(this->context)
	{
//		TRACE(<< "loading texture" << std::endl)
		this->tex = this->context->loader.load<morda::res::texture>("tex_sample");
	}

	uint32_t timer = 0;
	uint32_t cnt = 0;

	void update(uint32_t dt) override{
		this->timer += dt;
		++this->cnt;

		if(this->timer > 1000){
			this->timer -= 1000;

			TRACE(<< "Update(): UPS = " << this->cnt << std::endl)

			this->cnt = 0;
		}
	}

	bool on_mouse_button(const morda::mouse_button_event& e)override{
		TRACE(<< "OnMouseButton(): isDown = " << e.is_down << ", pos = " << e.pos << ", button = " << unsigned(e.button) << ", pointer_id = " << e.pointer_id << std::endl)
		if(!e.is_down){
			return false;
		}

		if(this->is_updating()){
			this->context->updater->stop(*this);
		}else{
			this->context->updater->start(
					utki::make_shared_from(*this),
					30
				);
		}
		this->focus();
		return true;
	}

	bool on_key(bool isDown, morda::key keyCode) override{
		if(isDown){
			TRACE(<< "SimpleWidget::OnKey(): down, keyCode = " << unsigned(keyCode) << std::endl)
			switch(keyCode){
				case morda::key::left:
					TRACE(<< "SimpleWidget::OnKeyDown(): LEFT key caught" << std::endl)
					return true;
				case morda::key::a:
					TRACE(<< "SimpleWidget::OnKeyUp(): A key caught" << std::endl)
					return true;
				default:
					break;
			}
		}else{
			TRACE(<< "SimpleWidget::OnKey(): up, keyCode = " << unsigned(keyCode) << std::endl)
			switch(keyCode){
				case morda::key::left:
					TRACE(<< "SimpleWidget::OnKeyUp(): LEFT key caught" << std::endl)
					return true;
				case morda::key::a:
					TRACE(<< "SimpleWidget::OnKeyUp(): A key caught" << std::endl)
					return true;
				default:
					break;
			}
		}
		return false;
	}

	void on_character_input(const std::u32string& unicode, morda::key key)override{
		if(unicode.size() == 0){
			return;
		}

		TRACE(<< "SimpleWidget::OnCharacterInput(): unicode = " << unicode[0] << std::endl)
	}

	void render(const morda::matrix4& matrix)const override{
		{
			morda::matrix4 matr(matrix);
			matr.scale(this->rect().d);

			auto& r = *this->context->renderer;
			r.shader->pos_tex->render(matr, *r.pos_tex_quad_01_vao, this->tex->tex());
		}

//		this->fnt->Fnt().RenderTex(s , matrix);

//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glEnable(GL_BLEND);
//		morda::SimpleTexturingShader &s = morda::gui::inst().shaders.simpleTexturing;
//		morda::matrix4 m(matrix);
//		m.translate(200, 200);
//		this->fnt->Fnt().RenderString(s, m, "Hello World!");
	}
};

class CubeWidget : public morda::widget, public morda::updateable{
	std::shared_ptr<morda::res::texture> tex;

	morda::quaternion rot = morda::quaternion().set_identity();
public:
	std::shared_ptr<morda::vertex_array> cubeVAO;

	CubeWidget(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			morda::widget(std::move(c), desc)
	{
		std::array<morda::vector3, 36> cubePos = {{
			morda::vector3(-1, -1,  1), morda::vector3( 1, -1,  1), morda::vector3(-1,  1,  1),
 			morda::vector3( 1, -1,  1), morda::vector3( 1,  1,  1), morda::vector3(-1,  1,  1),
			morda::vector3( 1, -1,  1), morda::vector3( 1, -1, -1), morda::vector3( 1,  1,  1),
			morda::vector3( 1, -1, -1), morda::vector3( 1,  1, -1), morda::vector3( 1,  1,  1),
			morda::vector3( 1, -1, -1), morda::vector3(-1, -1, -1), morda::vector3( 1,  1, -1),
			morda::vector3(-1, -1, -1), morda::vector3(-1,  1, -1), morda::vector3( 1,  1, -1),
			morda::vector3(-1, -1, -1), morda::vector3(-1, -1,  1), morda::vector3(-1,  1, -1),
			morda::vector3(-1, -1,  1), morda::vector3(-1,  1,  1), morda::vector3(-1,  1, -1),
			morda::vector3(-1,  1, -1), morda::vector3(-1,  1,  1), morda::vector3( 1,  1, -1),
			morda::vector3(-1,  1,  1), morda::vector3( 1,  1,  1), morda::vector3( 1,  1, -1),
			morda::vector3(-1, -1, -1), morda::vector3( 1, -1, -1), morda::vector3(-1, -1,  1),
			morda::vector3(-1, -1,  1), morda::vector3( 1, -1, -1), morda::vector3( 1, -1,  1)
		}};

		auto posVBO = this->context->renderer->factory->create_vertex_buffer(utki::make_span(cubePos));

		std::array<morda::vector2, 36> cubeTex = {{
			morda::vector2(0, 0), morda::vector2(1, 0), morda::vector2(0, 1),
			morda::vector2(1, 0), morda::vector2(1, 1), morda::vector2(0, 1),
			morda::vector2(0, 0), morda::vector2(1, 0), morda::vector2(0, 1),
			morda::vector2(1, 0), morda::vector2(1, 1), morda::vector2(0, 1),
			morda::vector2(0, 0), morda::vector2(1, 0), morda::vector2(0, 1),
			morda::vector2(1, 0), morda::vector2(1, 1), morda::vector2(0, 1),
			morda::vector2(0, 0), morda::vector2(1, 0), morda::vector2(0, 1),
			morda::vector2(1, 0), morda::vector2(1, 1), morda::vector2(0, 1),
			morda::vector2(0, 0), morda::vector2(1, 0), morda::vector2(0, 1),
			morda::vector2(1, 0), morda::vector2(1, 1), morda::vector2(0, 1),
			morda::vector2(0, 0), morda::vector2(1, 0), morda::vector2(0, 1),
			morda::vector2(1, 0), morda::vector2(1, 1), morda::vector2(0, 1)
		}};

		auto texVBO = this->context->renderer->factory->create_vertex_buffer(utki::make_span(cubeTex));

		std::array<uint16_t, 36> indices = {{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
		}};

		auto cubeIndices = this->context->renderer->factory->create_index_buffer(utki::make_span(indices));

		this->cubeVAO = this->context->renderer->factory->create_vertex_array({posVBO, texVBO}, cubeIndices, morda::vertex_array::mode::triangles);

		this->tex = this->context->loader.load<morda::res::texture>("tex_sample");
		this->rot.set_identity();
	}

	unsigned fps = 0;
	uint32_t fpsSecCounter = 0;

	void update(uint32_t dt) override{
		this->fpsSecCounter += dt;
		++this->fps;
		this->rot %= morda::quaternion().set_rotation(r4::vector3<float>(1, 2, 1).normalize(), 1.5f * (float(dt) / 1000));
		if(this->fpsSecCounter >= 1000){
			TRACE_ALWAYS(<< "fps = " << std::dec << fps << std::endl)
			this->fpsSecCounter = 0;
			this->fps = 0;
		}
		this->clear_cache();
	}

	void render(const morda::matrix4& matrix)const override{
		this->widget::render(matrix);
		
		morda::matrix4 matr(matrix);
		matr.scale(this->rect().d / 2);
		matr.translate(1, 1);
		matr.scale(1, -1);
		matr.frustum(-2, 2, -1.5, 1.5, 2, 100);
		matr.translate(0, 0, -4);
		matr.rotate(this->rot);

//		glEnable(GL_CULL_FACE);

		this->context->renderer->shader->pos_tex->render(matr, *this->cubeVAO, this->tex->tex());

//		glDisable(GL_CULL_FACE);
	}
};

class TreeViewItemsProvider : public morda::tree_view::provider{
	puu::forest root;

	std::shared_ptr<morda::context> context;
public:

	TreeViewItemsProvider(std::shared_ptr<morda::context> c) :
			context(c)
	{
		this->root = puu::read(R"qwertyuiop(
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
				root2{
					subsubroot1
					subsubroot2{
						trololo
						"hello world!"
					}
				}
				root3
				root4
			)qwertyuiop");
	}

	~TreeViewItemsProvider(){

	}

private:
	std::vector<size_t> selectedItem;

	unsigned newItemNumber = 0;

	std::string generateNewItemvalue(){
		std::stringstream ss;
		ss << "newItem" << newItemNumber;
		++newItemNumber;
		return ss.str();
	}

public:
	void insertBefore(){
		if(this->selectedItem.size() == 0){
			return;
		}

		puu::forest* list = &this->root;
		puu::forest* parent_list = nullptr;

		for(auto& i : this->selectedItem){
			parent_list = list;
			list = &(*list)[i].children;
		}

		if(!parent_list){
			return;
		}

		parent_list->insert(std::next(parent_list->begin(), this->selectedItem.back()), puu::leaf(this->generateNewItemvalue()));

		this->notify_item_added(this->selectedItem);
		++this->selectedItem.back();
	}

	void insertAfter(){
		if(this->selectedItem.size() == 0){
			return;
		}

		puu::forest* list = &this->root;
		puu::forest* parent_list = nullptr;

		for(auto& i : this->selectedItem){
			parent_list = list;
			list = &(*list)[i].children;
		}

		if(!parent_list){
			return;
		}

		parent_list->insert(std::next(parent_list->begin(), this->selectedItem.back() + 1), puu::leaf(this->generateNewItemvalue()));

		++this->selectedItem.back();
		this->notify_item_added(this->selectedItem);
		--this->selectedItem.back();
	}

	void insertChild(){
		if(this->selectedItem.size() == 0){
			return;
		}

		puu::forest* list = &this->root;

		for(auto& i : this->selectedItem){
			list = &(*list)[i].children;
		}

		list->push_back(puu::leaf(this->generateNewItemvalue()));

		this->selectedItem.push_back(list->size() - 1);
		this->notify_item_added(this->selectedItem);
		this->selectedItem.pop_back();
	}

	std::shared_ptr<morda::widget> get_widget(const std::vector<size_t>& path, bool isCollapsed)override{
		ASSERT(path.size() >= 1)

		auto list = &this->root;
		decltype(list) parent_list = nullptr;

		std::vector<bool> isLastItemInParent;

		puu::tree* n = nullptr;

		for(const auto& i : path){
			isLastItemInParent.push_back(i + 1 == list->size());
			n = &(*list)[i];
			parent_list = list;
			list = &n->children;
		}

		auto ret = std::make_shared<morda::row>(this->context, puu::forest());

		{
			auto v = this->context->inflater.inflate(
					R"qwertyuiop(
							@pile{
								@color{
									id{selection}
									layout{dx{max}dy{max}}
									color{${morda_color_highlight}}
									visible{false}
								}
								@text{
									id{value}
								}
								@mouse_proxy{
									id{mouse_proxy}
									layout{dx{max}dy{max}}
								}
							}
						)qwertyuiop"
				);

			{
				auto value = v->try_get_widget_as<morda::text>("value");
				ASSERT(value)
				value->set_text(n->value.to_string());
			}
			{
				auto colorLabel = v->try_get_widget_as<morda::color>("selection");

				colorLabel->set_visible(this->selectedItem == path);

				auto mp = v->try_get_widget_as<morda::mouse_proxy>("mouse_proxy");
				ASSERT(mp)
				mp->mouse_button_handler = [this, path](morda::mouse_proxy&, const morda::mouse_button_event& e) -> bool{
					if(!e.is_down || e.button != morda::mouse_button::left){
						return false;
					}

					this->selectedItem = path;
					TRACE(<< " selected item = ")
					for(auto& k : this->selectedItem){
						TRACE(<< k << ", ")
					}
					TRACE(<< std::endl)
					this->notify_item_changed();

					return true;
				};
			}

			ret->push_back(v);
		}

		{
			auto b = this->context->inflater.inflate_as<morda::push_button>(
					R"qwertyuiop(
							@push_button{
								@color{
									color{0xff0000ff}
									layout{dx{2mm}dy{0.5mm}}
								}
							}
						)qwertyuiop"
				);
			b->click_handler = [this, path, parent_list](morda::push_button& button){
				ASSERT(parent_list)
				parent_list->erase(std::next(parent_list->begin(), path.back()));
				this->notify_item_removed(path);
			};
			ret->push_back(b);
		}

		return ret;
	}

	size_t count(const std::vector<size_t>& path) const noexcept override{
		auto children = &this->root;

		for(auto& i : path){
			children = &(*children)[i].children;
		}

		return children->size();
	}

};

class application : public mordavokne::application{
	static mordavokne::window_params GetWindowParams()noexcept{
		mordavokne::window_params wp(r4::vector2<unsigned>(1024, 800));

		return wp;
	}
public:
	application() :
			mordavokne::application("morda-tests", GetWindowParams())
	{
		this->gui.initStandardWidgets(*this->get_res_file("../../res/morda_res/"));

		this->gui.context->loader.mount_res_pack(*this->get_res_file("res/"));
//		this->ResMan().MountResPack(morda::ZipFile::New(papki::FSFile::New("res.zip")));

		this->gui.context->inflater.register_widget<SimpleWidget>("U_SimpleWidget");
		this->gui.context->inflater.register_widget<CubeWidget>("CubeWidget");

		std::shared_ptr<morda::widget> c = this->gui.context->inflater.inflate(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);

		std::dynamic_pointer_cast<morda::key_proxy>(c)->key_handler = [this](morda::key_proxy&, bool isDown, morda::key keyCode) -> bool {
			if(isDown){
				if(keyCode == morda::key::escape){
					this->quit();
				}
			}
			return false;
		};

//		morda::ZipFile zf(papki::FSFile::New("res.zip"), "test.gui.stob");
//		std::shared_ptr<morda::widget> c = morda::gui::inst().inflater().Inflate(zf);

		ASSERT(c->try_get_widget_as<morda::push_button>("show_VK_button"))
		std::dynamic_pointer_cast<morda::push_button>(c->try_get_widget("show_VK_button"))->click_handler = [this](morda::push_button&){
			this->show_virtual_keyboard();
		};

		std::dynamic_pointer_cast<morda::push_button>(c->try_get_widget("push_button_in_scroll_container"))->click_handler = [this](morda::push_button&){
			this->gui.context->run_from_ui_thread(
					[](){
						TRACE_ALWAYS(<< "Print from UI thread!!!!!!!!" << std::endl)
					}
				);
		};

		// cube click_proxy
		{
			auto cube = c->try_get_widget_as<CubeWidget>("cube_widget");
			ASSERT(cube)

			auto& cp = c->get_widget_as<morda::click_proxy>("cube_click_proxy");
			auto& bg = c->get_widget_as<morda::color>("cube_bg_color");
			cp.press_change_handler = [bg{utki::make_shared_from(bg)}](morda::click_proxy& w) -> bool {
				bg->set_color(w.is_pressed() ? 0xff808080 : 0x80808080);
				return true;
			};
			cp.press_change_handler(cp); // set initial color
			cp.click_handler = [cube](morda::click_proxy&) -> bool {
				if(cube->is_updating()){
					cube->context->updater->stop(*cube);
				}else{
					cube->context->updater->start(cube, 0);
				}
				return true;
			};
		}

		// scroll_area
		{
			auto scrollArea = c->try_get_widget_as<morda::scroll_area>("scroll_area");
			auto sa = utki::make_weak(scrollArea);

			auto vertSlider = c->try_get_widget_as<morda::scroll_bar>("scroll_area_vertical_slider");
			auto vs = utki::make_weak(vertSlider);

			auto horiSlider = c->try_get_widget_as<morda::scroll_bar>("scroll_area_horizontal_slider");
			auto hs = utki::make_weak(horiSlider);

			auto resizeProxy = c->try_get_widget_as<morda::resize_proxy>("scroll_area_resize_proxy");
			auto rp = utki::make_weak(resizeProxy);

			resizeProxy->resize_handler = [vs, hs, sa](morda::resize_proxy&){
				auto sc = sa.lock();
				if(!sc){
					return;
				}

				auto visibleArea = sc->get_visible_area_fraction();

				if(auto v = vs.lock()){
					v->set_fraction(sc->get_scroll_factor().y());
					v->set_band_fraction(visibleArea.y());
				}
				if(auto h = hs.lock()){
					h->set_fraction(sc->get_scroll_factor().x());
					h->set_band_fraction(visibleArea.x());
				}
			};
			resizeProxy->on_resize();

			vertSlider->fraction_change_handler = [sa](morda::fraction_widget& slider){
				if(auto s = sa.lock()){
					auto sf = s->get_scroll_factor();
					sf.y() = slider.fraction();
					s->set_scroll_factor(sf);
				}
			};

			horiSlider->fraction_change_handler = [sa](morda::fraction_widget& slider){
				if(auto s = sa.lock()){
					auto sf = s->get_scroll_factor();
					sf.x() = slider.fraction();
					s->set_scroll_factor(sf);
				}
			};
		}

		// VerticalList
		{
			auto verticalList = c->try_get_widget_as<morda::list>("list");
			auto vl = utki::make_weak(verticalList);

			auto verticalSlider = c->try_get_widget_as<morda::vertical_scroll_bar>("vertical_list_slider");
			auto vs = utki::make_weak(verticalSlider);

			verticalSlider->fraction_change_handler = [vl](morda::fraction_widget& slider){
				if(auto l = vl.lock()){
					l->set_scroll_factor(slider.fraction());
				}
			};

			auto resizeProxy = c->try_get_widget_as<morda::resize_proxy>("vertical_list_resize_proxy");
			ASSERT(resizeProxy)

			resizeProxy->resize_handler = [vs, vl](morda::resize_proxy&){
				auto l = vl.lock();
				if(!l){
					return;
				}
				if(auto s = vs.lock()){
					s->set_fraction(l->get_scroll_factor());
				}
			};

			auto mouseProxy = c->try_get_widget_as<morda::mouse_proxy>("list_mouseproxy");
			struct State : public utki::shared{
				morda::vector2 oldPos = 0;
				bool isLeftButtonPressed;
			};
			auto state = std::make_shared<State>();

			mouseProxy->mouse_button_handler = [state](morda::mouse_proxy&, const morda::mouse_button_event& e){
				if(e.button == morda::mouse_button::left){
					state->isLeftButtonPressed = e.is_down;
					state->oldPos = e.pos;
					return true;
				}
				return false;
			};

			mouseProxy->mouse_move_handler = [vs, vl, state](morda::mouse_proxy&, const morda::mouse_move_event& e){
				if(state->isLeftButtonPressed){
					auto dp = state->oldPos - e.pos;
					state->oldPos = e.pos;
					if(auto l = vl.lock()){
						l->scroll_by(dp.y());
						if(auto s = vs.lock()){
							s->set_fraction(l->get_scroll_factor());
						}
					}
					return true;
				}
				return false;
			};
		}

		// HorizontalList
		{
			auto horizontalList = c->try_get_widget_as<morda::list_widget>("pan_list");
			auto hl = utki::make_weak(horizontalList);

			auto horizontalSlider = c->try_get_widget_as<morda::fraction_widget>("horizontal_list_slider");
			ASSERT(horizontalSlider)
			auto hs = utki::make_weak(horizontalSlider);

			horizontalSlider->fraction_change_handler = [hl](morda::fraction_widget& slider){
//				TRACE(<< "horizontal slider factor = " << slider.factor() << std::endl)
				if(auto l = hl.lock()){
					l->set_scroll_factor(slider.fraction());
				}
			};

			auto resizeProxy = c->try_get_widget_as<morda::resize_proxy>("horizontal_list_resize_proxy");
			ASSERT(resizeProxy)

			resizeProxy->resize_handler = [hs, hl](morda::resize_proxy&){
				auto l = hl.lock();
				if(!l){
					return;
				}
				if(auto s = hs.lock()){
					s->set_fraction(l->get_scroll_factor());
				}
			};

			auto mouseProxy = c->try_get_widget_as<morda::mouse_proxy>("horizontal_list_mouseproxy");
			struct State : public utki::shared{
				morda::vector2 oldPos = 0;
				bool isLeftButtonPressed;
			};
			auto state = std::make_shared<State>();

			mouseProxy->mouse_button_handler = [state](morda::mouse_proxy&, const morda::mouse_button_event& e){
				if(e.button == morda::mouse_button::left){
					state->isLeftButtonPressed = e.is_down;
					state->oldPos = e.pos;
					return true;
				}
				return false;
			};

			mouseProxy->mouse_move_handler = [hl, hs, state](morda::mouse_proxy& w, const morda::mouse_move_event& e) -> bool {
				if(state->isLeftButtonPressed){
					auto dp = state->oldPos - e.pos;
					state->oldPos = e.pos;
					if(auto l = hl.lock()){
						l->scroll_by(dp.x());
						if(auto s = hs.lock()){
							s->set_fraction(l->get_scroll_factor());
						}
					}
					return true;
				}
				return false;
			};
		}

		// text_input
		{
			auto& l = c->get_widget("text_input").get_widget<morda::text_input_line>();
			ASSERT_ALWAYS(!l.get_text().empty())
		}

		// tree_view
		{
			auto treeview = c->try_get_widget_as<morda::tree_view>("treeview_widget");
			ASSERT(treeview)
			auto provider = std::make_shared<TreeViewItemsProvider>(c->context);
			treeview->set_provider(provider);
			auto tv = utki::make_weak(treeview);

			auto verticalSlider = c->try_get_widget_as<morda::vertical_scroll_bar>("treeview_vertical_slider");
			auto vs = utki::make_weak(verticalSlider);

			verticalSlider->fraction_change_handler = [tv](morda::fraction_widget& slider){
				if(auto t = tv.lock()){
					t->set_vertical_scroll_factor(slider.fraction());
				}
			};

			auto horizontalSlider = c->try_get_widget_as<morda::horizontal_scroll_bar>("treeview_horizontal_slider");
			ASSERT(horizontalSlider)
			auto hs = utki::make_weak(horizontalSlider);

			horizontalSlider->fraction_change_handler = [tv](morda::fraction_widget& slider){
				if(auto t = tv.lock()){
					t->set_horizontal_scroll_factor(slider.fraction());
				}
			};

			auto resizeProxy = c->try_get_widget_as<morda::resize_proxy>("treeview_resize_proxy");
			ASSERT(resizeProxy)
			auto rp = utki::make_weak(resizeProxy);

			resizeProxy->resize_handler = [vs, hs, tv](morda::resize_proxy& w){
				auto t = tv.lock();
				if(!t){
					return;
				}
				if(auto h = hs.lock()){
					h->set_fraction(t->get_scroll_factor().x());
				}
				if(auto v = vs.lock()){
					v->set_fraction(t->get_scroll_factor().y());
				}
			};

			treeview->view_change_handler = [rp](morda::tree_view&){
				if(auto r = rp.lock()){
					if(r->resize_handler){
						r->resize_handler(*r);
					}
				}
			};

			auto insertBeforeButton = c->try_get_widget_as<morda::push_button>("insert_before");
			auto insertAfterButton = c->try_get_widget_as<morda::push_button>("insert_after");
			auto insertChild = c->try_get_widget_as<morda::push_button>("insert_child");

			auto prvdr = utki::make_weak(provider);
			insertBeforeButton->click_handler = [prvdr](morda::push_button& b){
				if(auto p = prvdr.lock()){
					p->insertBefore();
				}
			};

			insertAfterButton->click_handler = [prvdr](morda::push_button& b){
				if(auto p = prvdr.lock()){
					p->insertAfter();
				}
			};

			insertChild->click_handler = [prvdr](morda::push_button& b){
				if(auto p = prvdr.lock()){
					p->insertChild();
				}
			};
		}

		// fullscreen
		{
			auto b = c->try_get_widget_as<morda::push_button>("fullscreen_button");
			b->click_handler = [this](morda::push_button&) {
				this->set_fullscreen(!this->is_fullscreen());
			};
		}
		{
			auto b = c->try_get_widget_as<morda::push_button>("image_push_button");
			ASSERT(b)
			b->click_handler = [this](morda::push_button&) {
				this->set_fullscreen(true);
			};
		}

		// mouse cursor
		{
			auto b = c->try_get_widget_as<morda::push_button>("showhide_mousecursor_button");
			bool visible = true;
			this->set_mouse_cursor_visible(visible);
			b->click_handler = [visible, this](morda::push_button&) mutable{
				visible = !visible;
				this->set_mouse_cursor_visible(visible);
			};
		}

		// dropdown
		{
			auto dds = c->try_get_widget_as<morda::drop_down_box>("dropdownselector");
			auto ddst = c->try_get_widget_as<morda::text>("dropdownselector_selection");
			auto ddstw = utki::make_weak(ddst);

			dds->selection_handler = [ddstw](morda::drop_down_box& dds){
				if(auto t = ddstw.lock()){
					std::stringstream ss;
					ss << "index_" << dds.get_selection();

					t->set_text(ss.str());
				}
			};
		}
	}
};

std::unique_ptr<mordavokne::application> mordavokne::create_application(int argc, const char** argv){
	return std::make_unique<::application>();
}
