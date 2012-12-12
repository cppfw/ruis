#include "morda/App.hpp"
#include "morda/Widget.hpp"
#include "morda/Container.hpp"

#include "morda/widgets/Button.hpp"
#include "morda/widgets/Label.hpp"

#include "morda/shaders/SimpleSingleColoringShader.hpp"
#include "morda/shaders/SimpleTexturingShader.hpp"

#include "morda/resources/ResTexture.hpp"
#include "morda/resources/ResFont.hpp"


#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>



class SimpleWidget : public morda::Widget, public morda::Updateable{
	ting::Ref<morda::ResTexture> tex;
	ting::Ref<morda::ResFont> fnt;
	
	SimpleWidget(const stob::Node& description) :
			morda::Widget(description, false)
	{
//		TRACE(<< "loading texture" << std::endl)
		this->tex = morda::App::Inst().ResMan().Load<morda::ResTexture>("tex_sample");
		this->fnt = morda::App::Inst().ResMan().Load<morda::ResFont>("fnt_main");
	}
public:	
	static inline ting::Ref<SimpleWidget> New(const stob::Node& description){
		return ting::Ref<SimpleWidget>(
				new SimpleWidget(description)
			);
	}
	
	ting::Inited<ting::u32, 0> timer;
	ting::Inited<ting::u32, 0> cnt;
	
	//override
	void Update(ting::u32 dt){
		this->timer += dt;
		++this->cnt;
		
		if(this->timer > 1000){
			this->timer -= 1000;
			
			TRACE(<< "Update(): UPS = " << this->cnt << std::endl)
			
			this->cnt = 0;
		}
	}
	
	//override
	bool OnMouseButtonDown(const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
//		TRACE(<< "OnMouseButtonDown(): invoked" << std::endl)
		if(this->IsUpdating()){
			this->StopUpdating();
		}else{
			this->StartUpdating(30);
		}
		this->SetDeliverCharacterInputEvents(true);
		this->Focus();
		return true;
	}
	
	//override
	bool OnKeyDown(morda::key::Key keyCode){
		TRACE(<< "SimpleWidget::OnKeyDown(): keyCode = " << unsigned(keyCode) << std::endl)
		switch(keyCode){
			case morda::key::LEFT:
				TRACE(<< "SimpleWidget::OnKeyDown(): LEFT key caught" << std::endl)
				return true;
			case morda::key::A:
				TRACE(<< "SimpleWidget::OnKeyUp(): A key caught" << std::endl)
				return true;
			default:
				break;
		}
		return false;
	}
	
	//override
	bool OnKeyUp(morda::key::Key keyCode){
		TRACE(<< "SimpleWidget::OnKeyUp(): keyCode = " << unsigned(keyCode) << std::endl)
		switch(keyCode){
			case morda::key::LEFT:
				TRACE(<< "SimpleWidget::OnKeyUp(): LEFT key caught" << std::endl)
				return true;
			case morda::key::A:
				TRACE(<< "SimpleWidget::OnKeyUp(): A key caught" << std::endl)
				return true;
			default:
				break;
		}
		return false;
	}
	
	//override
	bool OnCharacterInput(const ting::Buffer<const ting::u32>& unicode){
		TRACE(<< "SimpleWidget::OnCharacterInput(): unicode = " << unicode[0] << std::endl)
		return true;
	}
	
	//override
	void Render(const morda::Matr4f& matrix)const{
		{
			morda::Matr4f matr(matrix);
			matr.Scale(this->Rect().d);

			this->tex->Tex().Bind();

			morda::SimpleTexturingShader &s = morda::App::Inst().Shaders().simpleTexturing;
			s.Bind();
			s.EnablePositionPointer();
//			s.SetColor(morda::Vec3f(1, 0, 0));
			s.SetMatrix(matr);
			s.DrawQuad01();
		}
		
//		this->fnt->Fnt().RenderTex(s , matrix);
		
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glEnable(GL_BLEND);
//		morda::SimpleTexturingShader &s = morda::App::Inst().Shaders().simpleTexturing;
//		morda::Matr4f m(matrix);
//		m.Translate(200, 200);
//		this->fnt->Fnt().RenderString(s, m, "Hello World!");
	}
};



class SimpleWidgetFactory : public morda::GuiInflater::WidgetFactory{
public:
	//override
	ting::Ref<morda::Widget> Create(const stob::Node& node)const{
		return SimpleWidget::New(node);
	}
};



class Application : public morda::App{
public:
	Application() :
			App(320, 480)
	{
		this->ResMan().MountResPack(this->CreateResourceFileInterface());
		
		this->Inflater().AddWidgetFactory("U_SimpleWidget", ting::Ptr<morda::GuiInflater::WidgetFactory>(new SimpleWidgetFactory()));

		ting::Ref<morda::Container> c = morda::App::Inst().Inflater().Inflate(
				*this->CreateResourceFileInterface("test.gui.stob")
			);
		
		class CustomKeyListener : public morda::KeyListener{
		public:
			//override
			bool OnKeyDown(morda::key::Key keyCode){
				TRACE(<< "CustomKeyListener::OnKeyDown(): keyCode = " << unsigned(keyCode) << std::endl)
				return false;
			}
			
			//override
			bool OnKeyUp(morda::key::Key keyCode){
				TRACE(<< "CustomKeyListener::OnKeyUp(): keyCode = " << unsigned(keyCode) << std::endl)
				return false;
			}
		};
		
		c->SetKeyListener(ting::Ptr<morda::KeyListener>(
				new CustomKeyListener()
			));
		
		this->SetRootContainer(c);
	}
};



ting::Ptr<morda::App> morda::CreateApp(int argc, char** argv, const ting::Buffer<const ting::u8>& savedState){
	return ting::Ptr<Application>(new Application());
}
