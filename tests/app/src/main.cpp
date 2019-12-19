#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/FSFile.hpp>

#include "mordavokne/AppFactory.hpp"

#include "../../../src/morda/config.hpp"

#include "../../../src/morda/widgets/widget.hpp"
#include "../../../src/morda/widgets/container.hpp"
#include "../../../src/morda/widgets/proxy/KeyProxy.hpp"

#include "../../../src/morda/widgets/button/PushButton.hpp"
#include "../../../src/morda/widgets/label/Text.hpp"

#include "../../../src/morda/res/ResTexture.hpp"
#include "../../../src/morda/res/ResFont.hpp"

#include "../../../src/morda/widgets/CharInputWidget.hpp"
#include "../../../src/morda/widgets/group/ScrollArea.hpp"
#include "../../../src/morda/widgets/group/Row.hpp"
#include "../../../src/morda/widgets/proxy/MouseProxy.hpp"
#include "../../../src/morda/widgets/slider/ScrollBar.hpp"
#include "../../../src/morda/widgets/group/List.hpp"
#include "../../../src/morda/widgets/group/TreeView.hpp"
#include "../../../src/morda/widgets/proxy/MouseProxy.hpp"
#include "../../../src/morda/widgets/proxy/ResizeProxy.hpp"
#include "../../../src/morda/widgets/label/Color.hpp"
#include "../../../src/morda/widgets/label/Image.hpp"

#include "../../../src/morda/util/ZipFile.hpp"
#include "morda/widgets/button/DropDownSelector.hpp"



class SimpleWidget : virtual public morda::Widget, public morda::Updateable, public morda::CharInputWidget{
	std::shared_ptr<morda::ResTexture> tex;

public:
	SimpleWidget(const stob::Node* desc) :
			morda::Widget(desc)
	{
//		TRACE(<< "loading texture" << std::endl)
		this->tex = morda::Morda::inst().resMan.load<morda::ResTexture>("tex_sample");
	}

	std::uint32_t timer = 0;
	std::uint32_t cnt = 0;

	void update(std::uint32_t dt) override{
		this->timer += dt;
		++this->cnt;

		if(this->timer > 1000){
			this->timer -= 1000;

			TRACE(<< "Update(): UPS = " << this->cnt << std::endl)

			this->cnt = 0;
		}
	}

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, morda::MouseButton_e button, unsigned pointerId) override{
		TRACE(<< "OnMouseButton(): isDown = " << isDown << ", pos = " << pos << ", button = " << unsigned(button) << ", pointerId = " << pointerId << std::endl)
		if(!isDown){
			return false;
		}

		if(this->isUpdating()){
			this->stopUpdating();
		}else{
			this->startUpdating(30);
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

	void onCharacterInput(const std::u32string& unicode, morda::key key) override{
		if(unicode.size() == 0){
			return;
		}

		TRACE(<< "SimpleWidget::OnCharacterInput(): unicode = " << unicode[0] << std::endl)
	}

	void render(const morda::Matr4r& matrix)const override{
		{
			morda::Matr4r matr(matrix);
			matr.scale(this->rect().d);

			auto& r = morda::inst().renderer();
			r.shader->posTex->render(matr, *r.posTexQuad01VAO, this->tex->tex());
		}

//		this->fnt->Fnt().RenderTex(s , matrix);

//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glEnable(GL_BLEND);
//		morda::SimpleTexturingShader &s = morda::Morda::inst().shaders.simpleTexturing;
//		morda::Matr4r m(matrix);
//		m.translate(200, 200);
//		this->fnt->Fnt().RenderString(s, m, "Hello World!");
	}
};



class CubeWidget : public morda::Widget, public morda::Updateable{
	std::shared_ptr<morda::ResTexture> tex;

	morda::Quatr rot = morda::Quatr().identity();
public:
	std::shared_ptr<morda::VertexArray> cubeVAO;

	CubeWidget(const stob::Node* desc) :
			morda::Widget(desc)
	{
		std::array<morda::Vec3r, 36> cubePos = {{
			r4::vec3f(-1, -1, 1), r4::vec3f(1, -1, 1), r4::vec3f(-1, 1, 1),
			r4::vec3f(1, -1, 1), r4::vec3f(1, 1, 1), r4::vec3f(-1, 1, 1),

			r4::vec3f(1, -1, 1), r4::vec3f(1, -1, -1), r4::vec3f(1, 1, 1),
			r4::vec3f(1, -1, -1), r4::vec3f(1, 1, -1), r4::vec3f(1, 1, 1),

			r4::vec3f(1, -1, -1), r4::vec3f(-1, -1, -1), r4::vec3f(1, 1, -1),
			r4::vec3f(-1, -1, -1), r4::vec3f(-1, 1, -1), r4::vec3f(1, 1, -1),

			r4::vec3f(-1, -1, -1), r4::vec3f(-1, -1, 1), r4::vec3f(-1, 1, -1),
			r4::vec3f(-1, -1, 1), r4::vec3f(-1, 1, 1), r4::vec3f(-1, 1, -1),

			r4::vec3f(-1, 1, -1), r4::vec3f(-1, 1, 1), r4::vec3f(1, 1, -1),
			r4::vec3f(-1, 1, 1), r4::vec3f(1, 1, 1), r4::vec3f(1, 1, -1),

			r4::vec3f(-1, -1, -1), r4::vec3f(1, -1, -1), r4::vec3f(-1, -1, 1),
			r4::vec3f(-1, -1, 1), r4::vec3f(1, -1, -1), r4::vec3f(1, -1, 1)
		}};

		auto posVBO = morda::inst().renderer().factory->createVertexBuffer(utki::wrapBuf(cubePos));

		std::array<r4::vec2f, 36> cubeTex = {{
			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),

			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),

			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),

			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),

			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),

			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1)
		}};

		auto texVBO = morda::inst().renderer().factory->createVertexBuffer(utki::wrapBuf(cubeTex));

		std::array<std::uint16_t, 36> indices = {{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
		}};

		auto cubeIndices = morda::inst().renderer().factory->createIndexBuffer(utki::wrapBuf(indices));

		this->cubeVAO = morda::inst().renderer().factory->createVertexArray({posVBO, texVBO}, cubeIndices, morda::VertexArray::Mode_e::TRIANGLES);

		this->tex = morda::Morda::inst().resMan.load<morda::ResTexture>("tex_sample");
		this->rot.identity();


	}

	unsigned fps = 0;
	std::uint32_t fpsSecCounter = 0;

	void update(std::uint32_t dt) override{
		this->fpsSecCounter += dt;
		++this->fps;
		this->rot %= morda::Quatr().initRot(r4::vec3f(1, 2, 1).normalize(), 1.5f * (float(dt) / 1000));
		if(this->fpsSecCounter >= 1000){
			TRACE_ALWAYS(<< "fps = " << std::dec << fps << std::endl)
			this->fpsSecCounter = 0;
			this->fps = 0;
		}
	}

	void render(const morda::Matr4r& matrix)const override{
		this->widget::render(matrix);

		morda::Matr4r matr(matrix);
		matr.scale(this->rect().d / 2);
		matr.scale(1, -1);
		matr.translate(1, -1);
		matr.frustum(-2, 2, -1.5, 1.5, 2, 100);

		morda::Matr4r m(matr);
		m.translate(0, 0, -4);

		m.rotate(this->rot);

//		glEnable(GL_CULL_FACE);

		morda::inst().renderer().shader->posTex->render(m, *this->cubeVAO, this->tex->tex());

//		glDisable(GL_CULL_FACE);
	}
};



class TreeViewItemsProvider : public morda::TreeView::ItemsProvider{
	std::unique_ptr<stob::Node> root;

public:

	TreeViewItemsProvider(){
		this->root = stob::parse(R"qwertyuiop(
				{
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
				}
			)qwertyuiop");
	}

	~TreeViewItemsProvider(){

	}

	const char* DPlusMinus = R"qwertyuiop(
			Pile{
				Image{
					id{plusminus}
				}
				MouseProxy{
					layout{
						dx{fill} dy{fill}
					}
					id{plusminus_mouseproxy}
				}
			}
		)qwertyuiop";

	const char* DLine = R"qwertyuiop(
			Pile{
				layout{dx{5mm} dy{fill}}
				Color{
					layout{dx{1pt}dy{fill}}
					color{${morda_color_highlight}}
				}
			}
		)qwertyuiop";

	const char* DLineEnd = R"qwertyuiop(
			Pile{
				layout{dx{5mm} dy{max}}
				Column{
					layout{dx{max}dy{max}}
					Color{
						layout{dx{1pt}dy{0}weight{1}}
						color{${morda_color_highlight}}
					}
					Widget{layout{dx{max}dy{0}weight{1}}}
				}
				Row{
					layout{dx{max}dy{max}}
					Widget{layout{dx{0}dy{max}weight{1}}}
					Color{
						layout{dx{0}dy{1pt}weight{1}}
						color{${morda_color_highlight}}
					}
				}
			}
		)qwertyuiop";

	const char* DLineMiddle = R"qwertyuiop(
			Pile{
				layout{dx{5mm} dy{max}}
				Color{
					layout{dx{1pt}dy{max}}
					color{${morda_color_highlight}}
				}
				Row{
					layout{dx{max}dy{max}}
					Widget{layout{dx{0}dy{max}weight{1}}}
					Color{
						layout{dx{0}dy{1pt}weight{1}}
						color{${morda_color_highlight}}
					}
				}
			}
		)qwertyuiop";

	const char* DEmpty = R"qwertyuiop(
			Widget{layout{dx{5mm}dy{0}}}
		)qwertyuiop";

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

		auto n = this->root.get();
		if(!n){
			return;
		}

		decltype(n) parent = nullptr;
		decltype(n) prev = nullptr;

		for(auto i = this->selectedItem.begin(); n && i != this->selectedItem.end(); ++i){
			parent = n;
			auto next = n->child(*i);

			n = next.get_node();
			prev = next.prev();
		}

		if(!n){
			return;
		}

		if(prev){
			prev->insertNext(utki::makeUnique<stob::Node>(this->generateNewItemvalue().c_str()));
		}else{
			parent->addAsFirstChild(this->generateNewItemvalue().c_str());
		}

		this->notifyItemAdded(this->selectedItem);
		++this->selectedItem.back();
	}

	void insertAfter(){
		if(this->selectedItem.size() == 0){
			return;
		}

		auto n = this->root.get();
		if(!n){
			return;
		}

		for(auto i = this->selectedItem.begin(); n && i != this->selectedItem.end(); ++i){
			auto next = n->child(*i);
			n = next.get_node();
		}

		if(!n){
			return;
		}

		n->insertNext(utki::makeUnique<stob::Node>(this->generateNewItemvalue().c_str()));

		++this->selectedItem.back();
		this->notifyItemAdded(this->selectedItem);
		--this->selectedItem.back();
	}

	void insertChild(){
		auto n = this->root.get();
		if(!n){
			return;
		}

		for(auto i = this->selectedItem.begin(); n && i != this->selectedItem.end(); ++i){
			auto next = n->child(*i);
			n = next.get_node();
		}

		if(!n || this->selectedItem.size() == 0){
			this->selectedItem.clear();
			n = this->root.get();
		}

		n->addAsFirstChild(this->generateNewItemvalue().c_str());

		this->selectedItem.push_back(0);
		this->notifyItemAdded(this->selectedItem);
		this->selectedItem.pop_back();
	}

	std::shared_ptr<morda::Widget> getWidget(const std::vector<size_t>& path, bool isCollapsed)override{
		ASSERT(path.size() >= 1)

		auto n = this->root.get();
		decltype(n) parent = nullptr;

		std::vector<bool> isLastItemInParent;

		for(auto i = path.begin(); i != path.end(); ++i){
			parent = n;
			n = n->child(*i).get_node();
			isLastItemInParent.push_back(n->next() == nullptr);
		}

		auto ret = std::make_shared<morda::Row>(nullptr);

		ASSERT(isLastItemInParent.size() == path.size())

		for(unsigned i = 0; i != path.size() - 1; ++i){
			ret->add(*(isLastItemInParent[i] ? stob::parse(DEmpty) : stob::parse(DLine)));
		}

		{
			auto widget = morda::Morda::inst().inflater.inflate_as<morda::Pile>(isLastItemInParent.back() ? DLineEnd : DLineMiddle);
			ASSERT(widget)

			if(n->child()){
				auto w = morda::Morda::inst().inflater.inflate(DPlusMinus);

				auto plusminus = w->try_get_widget_as<morda::Image>("plusminus");
				ASSERT(plusminus)
				plusminus->setImage(
						isCollapsed ?
								morda::Morda::inst().resMan.load<morda::ResImage>("morda_img_treeview_plus") :
								morda::Morda::inst().resMan.load<morda::ResImage>("morda_img_treeview_minus")
					);

				auto plusminusMouseProxy = w->try_get_widget_as<morda::MouseProxy>("plusminus_mouseproxy");
				ASSERT(plusminusMouseProxy)
				plusminusMouseProxy->mouseButton = [this, path, isCollapsed](morda::Widget& widget, bool isDown, const morda::Vec2r& pos, morda::MouseButton_e button, unsigned pointerId) -> bool{
					if(button != morda::MouseButton_e::LEFT){
						return false;
					}
					if(!isDown){
						return false;
					}

					if(isCollapsed){
						this->uncollapse(path);
					}else{
						this->collapse(path);
					}

					TRACE_ALWAYS(<< "plusminus clicked:")
					for(auto i = path.begin(); i != path.end(); ++i){
						TRACE_ALWAYS(<< " " << (*i))
					}
					TRACE_ALWAYS(<< std::endl)

					return true;
				};
				widget->push_back(w);
			}
			ret->push_back(widget);
		}

		{
			auto v = morda::Morda::inst().inflater.inflate(
					R"qwertyuiop(
							Pile{
								Color{
									id{selection}
									layout{dx{max}dy{max}}
									color{${morda_color_highlight}}
									visible{false}
								}
								Text{
									id{value}
								}
								MouseProxy{
									id{mouse_proxy}
									layout{dx{max}dy{max}}
								}
							}
						)qwertyuiop"
				);

			{
				auto value = v->try_get_widget_as<morda::Text>("value");
				ASSERT(value)
				value->setText(n->value());
			}
			{
				auto colorLabel = v->try_get_widget_as<morda::Color>("selection");

				colorLabel->set_visible(this->selectedItem == path);

				auto mp = v->try_get_widget_as<morda::MouseProxy>("mouse_proxy");
				ASSERT(mp)
				mp->mouseButton = [this, path](morda::Widget&, bool isDown, const morda::Vec2r&, morda::MouseButton_e button, unsigned pointerId) -> bool{
					if(!isDown || button != morda::MouseButton_e::LEFT){
						return false;
					}

					this->selectedItem = path;
					this->notifyItemChanged();
					//TODO:

					return true;
				};
			}

			ret->push_back(v);
		}

		{
			auto b = morda::Morda::inst().inflater.inflate_as<morda::PushButton>(
					R"qwertyuiop(
							PushButton{
								Color{
									color{0xff0000ff}
									layout{dx{2mm}dy{0.5mm}}
								}
							}
						)qwertyuiop"
				);
			b->clicked = [this, path, n, parent](morda::PushButton& button){
				ASSERT(parent)
				parent->removeChild(n);
				this->notifyItemRemoved(path);
			};
			ret->push_back(b);
		}

		return ret;
	}

	size_t count(const std::vector<size_t>& path) const noexcept override{
		auto n = this->root.get();

		for(auto i = path.begin(); i != path.end(); ++i){
			n = n->child(*i).get_node();
		}

		return n->count();
	}

};



class Application : public mordavokne::App{
	static mordavokne::App::WindowParams GetWindowParams()noexcept{
		mordavokne::App::WindowParams wp(r4::vec2ui(1024, 800));

		return wp;
	}
public:
	Application() :
			mordavokne::App("morda-tests", GetWindowParams())
	{
		morda::Morda::inst().initStandardWidgets(*this->getResFile("../../res/morda_res/"));

		morda::Morda::inst().resMan.mountResPack(*this->getResFile("res/"));
//		this->ResMan().MountResPack(morda::ZipFile::New(papki::FSFile::New("res.zip")));

		morda::Morda::inst().inflater.register_widget<SimpleWidget>("U_SimpleWidget");
		morda::Morda::inst().inflater.register_widget<CubeWidget>("CubeWidget");

		std::shared_ptr<morda::Widget> c = morda::Morda::inst().inflater.inflate(
				*this->getResFile("res/test.gui")
			);
		morda::Morda::inst().setRootWidget(c);

		std::dynamic_pointer_cast<morda::KeyProxy>(c)->key = [this](bool isDown, morda::key keyCode) -> bool{
			if(isDown){
				if(keyCode == morda::key::escape){
					this->quit();
				}
			}
			return false;
		};

//		morda::ZipFile zf(papki::FSFile::New("res.zip"), "test.gui.stob");
//		std::shared_ptr<morda::Widget> c = morda::Morda::inst().inflater().Inflate(zf);

		ASSERT(c->try_get_widget_as<morda::PushButton>("show_VK_button"))
		std::dynamic_pointer_cast<morda::PushButton>(c->try_get_widget("show_VK_button"))->clicked = [this](morda::PushButton&){
			this->show_virtual_keyboard();
		};

		std::dynamic_pointer_cast<morda::PushButton>(c->try_get_widget("push_button_in_scroll_container"))->clicked = [](morda::PushButton&){
			morda::Morda::inst().postToUiThread(
					[](){
						TRACE_ALWAYS(<< "Print from UI thread!!!!!!!!" << std::endl)
					}
				);
		};

		std::dynamic_pointer_cast<CubeWidget>(c->try_get_widget("cube_widget"))->startUpdating(0);

		//ScrollArea
		{
			auto scrollArea = c->try_get_widget_as<morda::ScrollArea>("scroll_area");
			auto sa = utki::makeWeak(scrollArea);

			auto vertSlider = c->try_get_widget_as<morda::ScrollBar>("scroll_area_vertical_slider");
			auto vs = utki::makeWeak(vertSlider);

			auto horiSlider = c->try_get_widget_as<morda::ScrollBar>("scroll_area_horizontal_slider");
			auto hs = utki::makeWeak(horiSlider);

			auto resizeProxy = c->try_get_widget_as<morda::ResizeProxy>("scroll_area_resize_proxy");
			auto rp = utki::makeWeak(resizeProxy);

			resizeProxy->resized = [vs, hs, sa](const morda::Vec2r& newSize){
				auto sc = sa.lock();
				if(!sc){
					return;
				}

				auto visibleArea = sc->visibleAreaFraction();

				if(auto v = vs.lock()){
					v->setFraction(sc->scrollFactor().y);
					v->setBandSizeFraction(visibleArea.y);
				}
				if(auto h = hs.lock()){
					h->setFraction(sc->scrollFactor().x);
					h->setBandSizeFraction(visibleArea.x);
				}
			};
			resizeProxy->resized(resizeProxy->rect().d);

			vertSlider->fractionChange = [sa](morda::FractionWidget& slider){
				if(auto s = sa.lock()){
					auto sf = s->scrollFactor();
					sf.y = slider.fraction();
					s->setScrollPosAsFactor(sf);
				}
			};

			horiSlider->fractionChange = [sa](morda::FractionWidget& slider){
				if(auto s = sa.lock()){
					auto sf = s->scrollFactor();
					sf.x = slider.fraction();
					s->setScrollPosAsFactor(sf);
				}
			};
		}

		//VerticalList
		{
			auto verticalList = c->try_get_widget_as<morda::VList>("vertical_list");
			auto vl = utki::makeWeak(verticalList);

			auto verticalSlider = c->try_get_widget_as<morda::VScrollBar>("vertical_list_slider");
			auto vs = utki::makeWeak(verticalSlider);

			verticalSlider->fractionChange = [vl](morda::FractionWidget& slider){
				if(auto l = vl.lock()){
					l->setScrollPosAsFactor(slider.fraction());
				}
			};

			auto resizeProxy = c->try_get_widget_as<morda::ResizeProxy>("vertical_list_resize_proxy");
			ASSERT(resizeProxy)

			resizeProxy->resized = [vs, vl](const morda::Vec2r& newSize){
				auto l = vl.lock();
				if(!l){
					return;
				}
				if(auto s = vs.lock()){
					s->setFraction(l->scrollFactor());
				}
			};

			auto mouseProxy = c->try_get_widget_as<morda::MouseProxy>("list_mouseproxy");
			struct State : public utki::Shared{
				morda::Vec2r oldPos = 0;
				bool isLeftButtonPressed;
			};
			auto state = std::make_shared<State>();

			mouseProxy->mouseButton = [state](morda::Widget& w, bool isDown, const morda::Vec2r& pos, morda::MouseButton_e button, unsigned id){
				if(button == morda::MouseButton_e::LEFT){
					state->isLeftButtonPressed = isDown;
					state->oldPos = pos;
					return true;
				}
				return false;
			};

			mouseProxy->mouseMove = [vs, vl, state](morda::Widget& w, const morda::Vec2r& pos, unsigned id){
				if(state->isLeftButtonPressed){
					auto dp = state->oldPos - pos;
					state->oldPos = pos;
					if(auto l = vl.lock()){
						l->scrollBy(dp.y);
						if(auto s = vs.lock()){
							s->setFraction(l->scrollFactor());
						}
					}
					return true;
				}
				return false;
			};
		}

		//HorizontalList
		{
			auto horizontalList = c->try_get_widget_as<morda::List>("horizontal_list");
			auto hl = utki::makeWeak(horizontalList);

			auto horizontalSlider = c->try_get_widget_as<morda::FractionWidget>("horizontal_list_slider");
			ASSERT(horizontalSlider)
			auto hs = utki::makeWeak(horizontalSlider);

			horizontalSlider->fractionChange = [hl](morda::FractionWidget& slider){
//				TRACE(<< "horizontal slider factor = " << slider.factor() << std::endl)
				if(auto l = hl.lock()){
					l->setScrollPosAsFactor(slider.fraction());
				}
			};

			auto resizeProxy = c->try_get_widget_as<morda::ResizeProxy>("horizontal_list_resize_proxy");
			ASSERT(resizeProxy)

			resizeProxy->resized = [hs, hl](const morda::Vec2r& newSize){
				auto l = hl.lock();
				if(!l){
					return;
				}
				if(auto s = hs.lock()){
					s->setFraction(l->scrollFactor());
				}
			};

			auto mouseProxy = c->try_get_widget_as<morda::MouseProxy>("horizontal_list_mouseproxy");
			struct State : public utki::Shared{
				morda::Vec2r oldPos = 0;
				bool isLeftButtonPressed;
			};
			auto state = std::make_shared<State>();

			mouseProxy->mouseButton = [state](morda::Widget& w, bool isDown, const morda::Vec2r& pos, morda::MouseButton_e button, unsigned id){
				if(button == morda::MouseButton_e::LEFT){
					state->isLeftButtonPressed = isDown;
					state->oldPos = pos;
					return true;
				}
				return false;
			};

			mouseProxy->mouseMove = [hl, hs, state](morda::Widget& w, const morda::Vec2r& pos, unsigned id){
				if(state->isLeftButtonPressed){
					auto dp = state->oldPos - pos;
					state->oldPos = pos;
					if(auto l = hl.lock()){
						l->scrollBy(dp.x);
						if(auto s = hs.lock()){
							s->setFraction(l->scrollFactor());
						}
					}
					return true;
				}
				return false;
			};
		}

		//TreeView
		{
			auto treeview = c->try_get_widget_as<morda::TreeView>("treeview_widget");
			ASSERT(treeview)
			auto provider = std::make_shared<TreeViewItemsProvider>();
			treeview->setItemsProvider(provider);
			auto tv = utki::makeWeak(treeview);

			auto verticalSlider = c->try_get_widget_as<morda::VScrollBar>("treeview_vertical_slider");
			auto vs = utki::makeWeak(verticalSlider);

			verticalSlider->fractionChange = [tv](morda::FractionWidget& slider){
				if(auto t = tv.lock()){
					t->setVerticalScrollPosAsFactor(slider.fraction());
				}
			};

			auto horizontalSlider = c->try_get_widget_as<morda::HScrollBar>("treeview_horizontal_slider");
			ASSERT(horizontalSlider)
			auto hs = utki::makeWeak(horizontalSlider);

			horizontalSlider->fractionChange = [tv](morda::FractionWidget& slider){
				if(auto t = tv.lock()){
					t->setHorizontalScrollPosAsFactor(slider.fraction());
				}
			};

			auto resizeProxy = c->try_get_widget_as<morda::ResizeProxy>("treeview_resize_proxy");
			ASSERT(resizeProxy)
			auto rp = utki::makeWeak(resizeProxy);

			resizeProxy->resized = [vs, hs, tv](const morda::Vec2r& newSize){
				auto t = tv.lock();
				if(!t){
					return;
				}
				if(auto h = hs.lock()){
					h->setFraction(t->scrollFactor().x);
				}
				if(auto v = vs.lock()){
					v->setFraction(t->scrollFactor().y);
				}
			};

			treeview->viewChanged = [rp](morda::TreeView&){
				if(auto r = rp.lock()){
					if(r->resized){
						r->resized(morda::Vec2r());
					}
				}
			};


			auto insertBeforeButton = c->try_get_widget_as<morda::PushButton>("insert_before");
			auto insertAfterButton = c->try_get_widget_as<morda::PushButton>("insert_after");
			auto insertChild = c->try_get_widget_as<morda::PushButton>("insert_child");

			auto prvdr = utki::makeWeak(provider);
			insertBeforeButton->clicked = [prvdr](morda::PushButton& b){
				if(auto p = prvdr.lock()){
					p->insertBefore();
				}
			};

			insertAfterButton->clicked = [prvdr](morda::PushButton& b){
				if(auto p = prvdr.lock()){
					p->insertAfter();
				}
			};

			insertChild->clicked = [prvdr](morda::PushButton& b){
				if(auto p = prvdr.lock()){
					p->insertChild();
				}
			};
		}


		//fullscreen
		{
			auto b = c->try_get_widget_as<morda::PushButton>("fullscreen_button");
			b->clicked = [this](morda::PushButton&) {
				this->setFullscreen(!this->isFullscreen());
			};
		}
		{
			auto b = c->try_get_widget_as<morda::PushButton>("image_push_button");
			ASSERT(b)
			b->clicked = [this](morda::PushButton&) {
				this->setFullscreen(true);
			};
		}


		//mouse cursor
		{
			auto b = c->try_get_widget_as<morda::PushButton>("showhide_mousecursor_button");
			bool visible = true;
			this->setMouseCursorVisible(visible);
			b->clicked = [visible](morda::PushButton&) mutable{
				visible = !visible;
				mordavokne::App::inst().setMouseCursorVisible(visible);
			};
		}

		//dropdown
		{
			auto dds = c->try_get_widget_as<morda::DropDownSelector>("dropdownselector");
			auto ddst = c->try_get_widget_as<morda::Text>("dropdownselector_selection");
			auto ddstw = utki::makeWeak(ddst);

			dds->selectionChanged = [ddstw](morda::DropDownSelector& dds){
				if(auto t = ddstw.lock()){
					std::stringstream ss;
					ss << "index_" << dds.selectedItem();

					t->setText(ss.str());
				}
			};
		}
	}
};



std::unique_ptr<mordavokne::App> mordavokne::create_application(int argc, const char** argv){
	return utki::makeUnique<Application>();
}
