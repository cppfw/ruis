
#include "morda/util/Quaternion.hpp"

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



class CubeWidget : public morda::Widget, public morda::Updateable{
	ting::Ref<morda::ResTexture> tex;
	
	CubeWidget(){
		this->tex = morda::App::Inst().ResMan().Load<morda::ResTexture>("tex_sample");
		this->rot.Identity();
		this->StartUpdating(30);
	}
	
	morda::Quatf rot;
public:
	
	static ting::Ref<CubeWidget> New(){
		return ting::Ref<CubeWidget>(
				new CubeWidget()
			);
	}
	
	//override
	void Update(ting::u32 dt){
		this->rot %= morda::Quatf().InitRot(morda::Vec3f(1, 2, 1).Normalize(), 1.5 * (float(dt) / 1000));
	}
	
	//override
	void Render(const morda::Matr4f& matrix)const{
		this->Widget::Render(matrix);
		
		morda::Matr4f matr(matrix);
		matr.Scale(this->Rect().d / 2);
		matr.Translate(1, 1);
		matr.Frustum(-1, 1, -1, 1, 1, 100);
		
		morda::Matr4f m(matr);
		m.Translate(0, 0, -3);
		
		m.Rotate(this->rot);

		this->tex->Tex().Bind();
		
		morda::SimpleTexturingShader &s = morda::App::Inst().Shaders().simpleTexturing;
		s.Bind();
		s.EnablePositionPointer();
		s.EnableTexCoordPointer();
//		s.SetColor(morda::Vec3f(0, 1, 0));
		s.SetMatrix(m);
		
		
		static morda::Vec3f cubePos[] = {
			morda::Vec3f(-1, -1, -1), morda::Vec3f(1, -1, -1), morda::Vec3f(-1, 1, -1),
			morda::Vec3f(1, -1, -1), morda::Vec3f(1, 1, -1), morda::Vec3f(-1, 1, -1),
			
			morda::Vec3f(1, -1, -1), morda::Vec3f(1, -1, 1), morda::Vec3f(1, 1, -1),
			morda::Vec3f(1, -1, 1), morda::Vec3f(1, 1, 1), morda::Vec3f(1, 1, -1),
			
			morda::Vec3f(1, -1, 1), morda::Vec3f(-1, -1, 1), morda::Vec3f(1, 1, 1),
			morda::Vec3f(-1, -1, 1), morda::Vec3f(-1, 1, 1), morda::Vec3f(1, 1, 1)
			
		};
		s.SetPositionPointer(cubePos);
		
		static morda::Vec2f cubeTex[] = {
			morda::Vec2f(0, 1), morda::Vec2f(0, 0), morda::Vec2f(1, 1),
			morda::Vec2f(1, 1), morda::Vec2f(0, 0), morda::Vec2f(1, 0),
			
			morda::Vec2f(0, 1), morda::Vec2f(0, 0), morda::Vec2f(1, 1),
			morda::Vec2f(1, 1), morda::Vec2f(0, 0), morda::Vec2f(1, 0),
			
			morda::Vec2f(0, 1), morda::Vec2f(0, 0), morda::Vec2f(1, 1),
			morda::Vec2f(1, 1), morda::Vec2f(0, 0), morda::Vec2f(1, 0),
		};
		s.SetTexCoordPointer(cubeTex);
		
//		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		
		s.DrawArrays(GL_TRIANGLES, 18);
	}
};


class Application : public morda::App{
	inline static morda::App::WindowParams GetWindowParams()throw(){
		morda::App::WindowParams wp;
		
		wp.dim.x = 320;
		wp.dim.y = 480;
		
		return wp;
	}
public:
	Application() :
			App(GetWindowParams())
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
		
		c->FindChildByName("show_VK_button").DynamicCast<morda::AbstractButton>()->pressed.Connect(static_cast<morda::App*>(this), &morda::App::ShowVirtualKeyboard);
		
		{
			ting::Ref<CubeWidget> w = CubeWidget::New();
			w->Resize(morda::Vec2f(100, 200));
			c->Add(w);
		}
		
		this->SetRootContainer(c);
	}
};



ting::Ptr<morda::App> morda::CreateApp(int argc, char** argv, const ting::Buffer<const ting::u8>& savedState){
	return ting::Ptr<Application>(new Application());
}
