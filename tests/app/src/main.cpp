#include "../../../src/morda/util/Quaternion.hpp"

#include "../../../src/morda/AppFactory.hpp"
#include "../../../src/morda/widgets/Widget.hpp"
#include "../../../src/morda/widgets/containers/Container.hpp"
#include "../../../src/morda/widgets/containers/KeyProxyContainer.hpp"

#include "../../../src/morda/widgets/Button.hpp"
#include "../../../src/morda/widgets/labels/Label.hpp"

#include "../../../src/morda/resources/ResTexture.hpp"
#include "../../../src/morda/resources/ResFont.hpp"

#include "../../../src/morda/widgets/CharInputWidget.hpp"
#include "../../../src/morda/widgets/containers/ScrollContainer.hpp"
#include "../../../src/morda/widgets/containers/LinearContainer.hpp"
#include "../../../src/morda/widgets/Slider.hpp"
#include "../../../src/morda/widgets/List.hpp"
#include "../../../src/morda/widgets/TreeView.hpp"
#include "../../../src/morda/widgets/MouseProxy.hpp"
#include "../../../src/morda/widgets/ResizeProxy.hpp"

#include "../../../src/morda/render/Render.hpp"

#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>
#include <ting/util.hpp>

#include "../../../src/morda/util/ZipFile.hpp"




class SimpleWidget : virtual public morda::Widget, public morda::Updateable, public morda::CharInputWidget{
	std::shared_ptr<morda::ResTexture> tex;
	std::shared_ptr<morda::ResFont> fnt;
	
public:	
	SimpleWidget(const stob::Node* desc) :
			morda::Widget(desc)
	{
//		TRACE(<< "loading texture" << std::endl)
		this->tex = morda::App::Inst().resMan.Load<morda::ResTexture>("tex_sample");
		this->fnt = morda::App::Inst().resMan.Load<morda::ResFont>("morda_fnt_main");
	}
	
	std::uint32_t timer = 0;
	std::uint32_t cnt = 0;
	
	void Update(std::uint32_t dt) override{
		this->timer += dt;
		++this->cnt;
		
		if(this->timer > 1000){
			this->timer -= 1000;
			
			TRACE(<< "Update(): UPS = " << this->cnt << std::endl)
			
			this->cnt = 0;
		}
	}
	
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId) override{
		TRACE(<< "OnMouseButton(): isDown = " << isDown << ", pos = " << pos << ", button = " << unsigned(button) << ", pointerId = " << pointerId << std::endl)
		if(!isDown){
			return false;
		}
		
		if(this->IsUpdating()){
			this->StopUpdating();
		}else{
			this->StartUpdating(30);
		}
		this->focus();
		return true;
	}
	
	bool onKey(bool isDown, morda::EKey keyCode) override{
		if(isDown){
			TRACE(<< "SimpleWidget::OnKey(): down, keyCode = " << unsigned(keyCode) << std::endl)
			switch(keyCode){
				case morda::EKey::LEFT:
					TRACE(<< "SimpleWidget::OnKeyDown(): LEFT key caught" << std::endl)
					return true;
				case morda::EKey::A:
					TRACE(<< "SimpleWidget::OnKeyUp(): A key caught" << std::endl)
					return true;
				default:
					break;
			}
		}else{
			TRACE(<< "SimpleWidget::OnKey(): up, keyCode = " << unsigned(keyCode) << std::endl)
			switch(keyCode){
				case morda::EKey::LEFT:
					TRACE(<< "SimpleWidget::OnKeyUp(): LEFT key caught" << std::endl)
					return true;
				case morda::EKey::A:
					TRACE(<< "SimpleWidget::OnKeyUp(): A key caught" << std::endl)
					return true;
				default:
					break;
			}
		}
		return false;
	}
	
	void OnCharacterInput(ting::Buffer<const std::uint32_t> unicode, morda::EKey key) override{
		if(unicode.size() == 0){
			return;
		}
		
		TRACE(<< "SimpleWidget::OnCharacterInput(): unicode = " << unicode[0] << std::endl)
	}
	
	void render(const morda::Matr4r& matrix)const override{
		{
			morda::Matr4r matr(matrix);
			matr.Scale(this->rect().d);

			this->tex->Tex().bind();

			morda::PosTexShader &s = morda::App::Inst().Shaders().posTexShader;

//			s.SetColor(morda::Vec3f(1, 0, 0));
			s.SetMatrix(matr);
			s.render(morda::PosShader::quad01Fan, s.quadFanTexCoords);
		}
		
//		this->fnt->Fnt().RenderTex(s , matrix);
		
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glEnable(GL_BLEND);
//		morda::SimpleTexturingShader &s = morda::App::Inst().Shaders().simpleTexturing;
//		morda::Matr4r m(matrix);
//		m.Translate(200, 200);
//		this->fnt->Fnt().RenderString(s, m, "Hello World!");
	}
};



class CubeWidget : public morda::Widget, public morda::Updateable{
	std::shared_ptr<morda::ResTexture> tex;
	
	morda::Quatr rot = morda::Quatr().Identity();
public:
	CubeWidget(const stob::Node* desc) :
			Widget(desc)
	{
		this->tex = morda::App::Inst().resMan.Load<morda::ResTexture>("tex_sample");
		this->rot.Identity();
		
		
	}
	
	void Update(std::uint32_t dt) override{
		this->rot %= morda::Quatr().InitRot(morda::Vec3f(1, 2, 1).Normalize(), 1.5f * (float(dt) / 1000));
	}
	
	void render(const morda::Matr4r& matrix)const override{
		this->Widget::render(matrix);
		
		morda::Matr4r matr(matrix);
		matr.Scale(this->rect().d / 2);
		matr.Translate(1, 1);
		matr.Frustum(-2, 2, -1.5, 1.5, 2, 100);
		
		morda::Matr4r m(matr);
		m.Translate(0, 0, -4);
		
		m.Rotate(this->rot);

		this->tex->Tex().bind();
		
		morda::PosTexShader &s = morda::App::Inst().Shaders().posTexShader;

//		s.SetColor(morda::Vec3f(0, 1, 0));
		s.SetMatrix(m);
		
		
		static std::array<morda::Vec3r, 36> cubePos = {{
			morda::Vec3f(-1, -1, 1), morda::Vec3f(1, -1, 1), morda::Vec3f(-1, 1, 1),
			morda::Vec3f(1, -1, 1), morda::Vec3f(1, 1, 1), morda::Vec3f(-1, 1, 1),
			
			morda::Vec3f(1, -1, 1), morda::Vec3f(1, -1, -1), morda::Vec3f(1, 1, 1),
			morda::Vec3f(1, -1, -1), morda::Vec3f(1, 1, -1), morda::Vec3f(1, 1, 1),
			
			morda::Vec3f(1, -1, -1), morda::Vec3f(-1, -1, -1), morda::Vec3f(1, 1, -1),
			morda::Vec3f(-1, -1, -1), morda::Vec3f(-1, 1, -1), morda::Vec3f(1, 1, -1),
			
			morda::Vec3f(-1, -1, -1), morda::Vec3f(-1, -1, 1), morda::Vec3f(-1, 1, -1),
			morda::Vec3f(-1, -1, 1), morda::Vec3f(-1, 1, 1), morda::Vec3f(-1, 1, -1),
			
			morda::Vec3f(-1, 1, -1), morda::Vec3f(-1, 1, 1), morda::Vec3f(1, 1, -1),
			morda::Vec3f(-1, 1, 1), morda::Vec3f(1, 1, 1), morda::Vec3f(1, 1, -1),
			
			morda::Vec3f(-1, -1, -1), morda::Vec3f(1, -1, -1), morda::Vec3f(-1, -1, 1),
			morda::Vec3f(-1, -1, 1), morda::Vec3f(1, -1, -1), morda::Vec3f(1, -1, 1)
		}};
		
		static std::array<morda::Vec2f, 36> cubeTex = {{
			morda::Vec2f(0, 0), morda::Vec2f(1, 0), morda::Vec2f(0, 1),
			morda::Vec2f(1, 0), morda::Vec2f(1, 1), morda::Vec2f(0, 1),
			
			morda::Vec2f(0, 0), morda::Vec2f(1, 0), morda::Vec2f(0, 1),
			morda::Vec2f(1, 0), morda::Vec2f(1, 1), morda::Vec2f(0, 1),
			
			morda::Vec2f(0, 0), morda::Vec2f(1, 0), morda::Vec2f(0, 1),
			morda::Vec2f(1, 0), morda::Vec2f(1, 1), morda::Vec2f(0, 1),
		
			morda::Vec2f(0, 0), morda::Vec2f(1, 0), morda::Vec2f(0, 1),
			morda::Vec2f(1, 0), morda::Vec2f(1, 1), morda::Vec2f(0, 1),
			
			morda::Vec2f(0, 0), morda::Vec2f(1, 0), morda::Vec2f(0, 1),
			morda::Vec2f(1, 0), morda::Vec2f(1, 1), morda::Vec2f(0, 1),
			
			morda::Vec2f(0, 0), morda::Vec2f(1, 0), morda::Vec2f(0, 1),
			morda::Vec2f(1, 0), morda::Vec2f(1, 1), morda::Vec2f(0, 1)
		}};
		
		static std::array<std::uint16_t, 36> indices = {{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
		}};
		
		morda::Render::setCullEnabled(true);
		
		s.render(indices, cubePos, cubeTex, morda::Render::EMode::TRIANGLES);
	}
};



class TreeViewItemsProvider : public morda::TreeView::ItemsProvider{
	std::unique_ptr<stob::Node> root;
	
public:
	
	TreeViewItemsProvider(){
		this->root = stob::Parse(R"qwertyuiop(
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
			FrameContainer{
				layout{
					dimX{5mm} dimY{5mm}
				}

				ColorLabel{
					layout{dimX{3mm}dimY{3mm}}
					color{0xa0808080}
				}

				Label{
					name{plusminus}
				}
				MouseProxy{
					layout{
						dimX{max} dimY{max}
					}
					name{plusminus_mouseproxy}
				}
			}
		)qwertyuiop";
	
	const char* DLine = R"qwertyuiop(
			FrameContainer{
				layout{dimX{5mm} dimY{max}}
				ColorLabel{
					layout{dimX{0.5mm}dimY{max}}
					color{0xffff0000}
				}
			}
		)qwertyuiop";
	
	const char* DLineEnd = R"qwertyuiop(
			FrameContainer{
				layout{dimX{5mm} dimY{max}}
				VerticalContainer{
					layout{dimX{max}dimY{max}}
					ColorLabel{
						layout{dimX{0.5mm}dimY{0}weight{1}}
						color{0xffff0000}
					}
					Widget{layout{dimX{max}dimY{0}weight{1}}}
				}
				HorizontalContainer{
					layout{dimX{max}dimY{max}}
					Widget{layout{dimX{0}dimY{max}weight{1}}}
					ColorLabel{
						layout{dimX{0}dimY{0.5mm}weight{1}}
						color{0xffff0000}
					}
				}
			}
		)qwertyuiop";
	
	const char* DLineMiddle = R"qwertyuiop(
			FrameContainer{
				layout{dimX{5mm} dimY{max}}
				ColorLabel{
					layout{dimX{0.5mm}dimY{max}}
					color{0xffff0000}
				}
				HorizontalContainer{
					layout{dimX{max}dimY{max}}
					Widget{layout{dimX{0}dimY{max}weight{1}}}
					ColorLabel{
						layout{dimX{0}dimY{0.5mm}weight{1}}
						color{0xffff0000}
					}
				}
			}
		)qwertyuiop";
	
	const char* DEmpty = R"qwertyuiop(
			Widget{layout{dimX{5mm}dimY{0}}}
		)qwertyuiop";
	
	std::shared_ptr<morda::Widget> getWidget(const std::vector<size_t>& path, bool isCollapsed)override{
		ASSERT(path.size() >= 1)
		
		auto n = this->root.get();
		decltype(n) parent = nullptr;
		
		std::vector<bool> isLast;
		
		for(auto i = path.begin(); i != path.end(); ++i){
			parent = n;
			n = n->child(*i);
			isLast.push_back(n->Next() == nullptr);
		}
		
		auto ret = ting::New<morda::HorizontalContainer>();

		ASSERT(isLast.size() == path.size())
		
		for(unsigned i = 0; i != path.size() - 1; ++i){
			ret->Add(*(isLast[i] ? stob::Parse(DEmpty) : stob::Parse(DLine)));
		}
		
		{
			auto widget = std::dynamic_pointer_cast<morda::FrameContainer>(morda::App::Inst().inflater.Inflate(*stob::Parse(isLast.back() ? DLineEnd : DLineMiddle)));
			ASSERT(widget)
			
			if(n->Child()){
				auto w = morda::App::Inst().inflater.Inflate(*stob::Parse(DPlusMinus));

				auto plusminus = w->findChildByNameAs<morda::Label>("plusminus");
				ASSERT(plusminus)
				plusminus->setText(isCollapsed ? "+" : "-");

				auto plusminusMouseProxy = w->findChildByNameAs<morda::MouseProxy>("plusminus_mouseproxy");
				ASSERT(plusminusMouseProxy)
				plusminusMouseProxy->mouseButton = [this, path, isCollapsed](morda::Widget& widget, bool isDown, const morda::Vec2r& pos, morda::Widget::EMouseButton button, unsigned pointerId) -> bool{
					if(button != morda::Widget::EMouseButton::LEFT){
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
				widget->Add(w);
			}
			ret->Add(widget);
		}
		
		auto value = ting::New<morda::Label>();
		ASSERT(value)
		
		std::stringstream ss;
		
		ss << n->Value();
		
		value->setText(ss.str());
		
		ret->Add(value);
		
		{
			auto b = std::dynamic_pointer_cast<morda::PushButton>(morda::App::Inst().inflater.Inflate(*stob::Parse(
					R"qwertyuiop(
							PushButton{
								ColorLabel{
									color{0xff0000ff}
									layout{dimX{2mm}dimY{0.5mm}}
								}
							}
						)qwertyuiop"
				)));
			b->clicked = [this, path, n, parent](morda::PushButton& button){
				ASSERT(parent)
				parent->removeChild(n);
				this->notifyItemRemoved(path);
			};
			ret->Add(b);
		}
		
		return ret;
	}
	
	size_t count(const std::vector<size_t>& path) const noexcept override{
		auto n = this->root.get();
		
		for(auto i = path.begin(); i != path.end(); ++i){
			n = n->child(*i);
		}
		
		return n->count();
	}

};



class Application : public morda::App{
	static morda::App::WindowParams GetWindowParams()noexcept{
		morda::App::WindowParams wp;
		
		wp.dim.x = 1024;
		wp.dim.y = 800;
		
		return wp;
	}
public:
	Application() :
			App(GetWindowParams())
	{
		this->resMan.MountResPack(*this->CreateResourceFileInterface("res/"));
//		this->ResMan().MountResPack(morda::ZipFile::New(ting::fs::FSFile::New("res.zip")));
		
		this->inflater.AddWidget<SimpleWidget>("U_SimpleWidget");
		this->inflater.AddWidget<CubeWidget>("CubeWidget");

		std::shared_ptr<morda::Widget> c = morda::App::Inst().inflater.Inflate(
				*this->CreateResourceFileInterface("res/test.gui.stob")
			);
		this->SetRootWidget(c);
		
		std::dynamic_pointer_cast<morda::KeyProxyContainer>(c)->key = [this](bool isDown, morda::EKey keyCode) -> bool{
			if(isDown){
				if(keyCode == morda::EKey::ESCAPE){
					this->Quit();
				}
			}
			return false;
		};

//		morda::ZipFile zf(ting::fs::FSFile::New("res.zip"), "test.gui.stob");
//		std::shared_ptr<morda::Widget> c = morda::App::Inst().inflater().Inflate(zf);
		
		
		std::dynamic_pointer_cast<morda::PushButton>(c->findChildByName("show_VK_button"))->clicked = [this](morda::PushButton&){
			this->ShowVirtualKeyboard();
		};
		
		std::dynamic_pointer_cast<morda::PushButton>(c->findChildByName("push_button_in_scroll_container"))->clicked = [this](morda::PushButton&){
			this->PostToUIThread_ts(
					[](){
						TRACE_ALWAYS(<< "Print from UI thread!!!!!!!!" << std::endl)
					}
				);
		};
		
		std::dynamic_pointer_cast<CubeWidget>(c->findChildByName("cube_widget"))->StartUpdating(30);
		
		//ScrollContainer
		{
			auto scrollArea = c->findChildByNameAs<morda::ScrollContainer>("scroll_area");
			auto sa = ting::makeWeak(scrollArea);
			
			auto vertSlider = c->findChildByNameAs<morda::HandleSlider>("scroll_area_vertical_slider");
			auto vs = ting::makeWeak(vertSlider);
			
			auto horiSlider = c->findChildByNameAs<morda::HandleSlider>("scroll_area_horizontal_slider");
			auto hs = ting::makeWeak(horiSlider);
			
			auto resizeProxy = c->findChildByNameAs<morda::ResizeProxy>("scroll_area_resize_proxy");
			auto rp = ting::makeWeak(resizeProxy);
			
			resizeProxy->resized = [vs, hs, sa](const morda::Vec2r& newSize){
				auto sc = sa.lock();
				if(!sc){
					return;
				}
				
				if(auto v = vs.lock()){
					v->setFactor(sc->scrollFactor().y);
				}
				if(auto h = hs.lock()){
					h->setFactor(sc->scrollFactor().x);
				}
			};
			
			
			vertSlider->factorChange = [sa](morda::Slider& slider){
				if(auto s = sa.lock()){
					auto sf = s->scrollFactor();
					sf.y = slider.factor();
					s->setScrollPosAsFactor(sf);
				}
			};
			
			horiSlider->factorChange = [sa](morda::Slider& slider){
				if(auto s = sa.lock()){
					auto sf = s->scrollFactor();
					sf.x = slider.factor();
					s->setScrollPosAsFactor(sf);
				}
			};
		}
		
		//VerticalList
		{
			auto verticalList = c->findChildByNameAs<morda::VerticalList>("vertical_list");
			std::weak_ptr<morda::VerticalList> vl = verticalList;
			
			auto verticalSlider = c->findChildByNameAs<morda::VerticalSlider>("vertical_list_slider");
			std::weak_ptr<morda::VerticalSlider> vs = verticalSlider;
			
			verticalSlider->factorChange = [vl](morda::Slider& slider){
				if(auto l = vl.lock()){
					l->setScrollPosAsFactor(slider.factor());
				}
			};
			
			auto resizeProxy = c->findChildByNameAs<morda::ResizeProxy>("vertical_list_resize_proxy");
			ASSERT(resizeProxy)
			
			resizeProxy->resized = [vs, vl](const morda::Vec2r& newSize){
				auto l = vl.lock();
				if(!l){
					return;
				}
				if(auto s = vs.lock()){
//					auto f = std::move(s->factorChange);
					s->setFactor(l->scrollFactor());
//					s->factorChange = std::move(f);
				}
			};
		}
		
		//TreeView
		{
			auto treeview = c->findChildByNameAs<morda::TreeView>("treeview_widget");
			ASSERT(treeview)
			treeview->setItemsProvider(ting::New<TreeViewItemsProvider>());
			std::weak_ptr<morda::TreeView> tv = treeview;
			
			auto verticalSlider = c->findChildByNameAs<morda::VerticalSlider>("treeview_vertical_slider");
			std::weak_ptr<morda::VerticalSlider> vs = verticalSlider;
			
			verticalSlider->factorChange = [tv](morda::Slider& slider){
				if(auto t = tv.lock()){
					t->setVerticalScrollPosAsFactor(slider.factor());
				}
			};
			
			auto horizontalSlider = c->findChildByNameAs<morda::HorizontalSlider>("treeview_horizontal_slider");
			ASSERT(horizontalSlider)
			std::weak_ptr<morda::HorizontalSlider> hs = horizontalSlider;
			
			horizontalSlider->factorChange = [tv](morda::Slider& slider){
				if(auto t = tv.lock()){
					t->setHorizontalScrollPosAsFactor(slider.factor());
				}
			};
			
			auto resizeProxy = c->findChildByNameAs<morda::ResizeProxy>("treeview_resize_proxy");
			ASSERT(resizeProxy)
			auto rp = ting::makeWeak(resizeProxy);
			
			resizeProxy->resized = [vs, hs, tv](const morda::Vec2r& newSize){
				auto t = tv.lock();
				if(!t){
					return;
				}
				if(auto h = hs.lock()){
					h->setFactor(t->scrollFactor().x);
				}
				if(auto v = vs.lock()){
					v->setFactor(t->scrollFactor().y);
				}
			};
			
			treeview->viewChanged = [rp](morda::TreeView&){
				if(auto r = rp.lock()){
					if(r->resized){
						r->resized(morda::Vec2r());
					}
				}
			};
		}
	}
};



std::unique_ptr<morda::App> morda::CreateApp(int argc, const char** argv, ting::Buffer<const std::uint8_t> savedState){
	return std::unique_ptr<Application>(new Application());
}
