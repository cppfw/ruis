#include "../../../src/morda/util/Quaternion.hpp"

#include "../../../src/morda/AppFactory.hpp"
#include "../../../src/morda/widgets/Widget.hpp"
#include "../../../src/morda/widgets/Container.hpp"

#include "../../../src/morda/widgets/Button.hpp"
#include "../../../src/morda/widgets/Label.hpp"

#include "../../../src/morda/shaders/SimpleSingleColoringShader.hpp"
#include "../../../src/morda/shaders/SimpleTexturingShader.hpp"

#include "../../../src/morda/resources/ResTexture.hpp"
#include "../../../src/morda/resources/ResFont.hpp"

#include "../../../src/morda/util/CharInputFocusable.hpp"

#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>
#include <ting/util.hpp>

#include "../../../src/morda/util/ZipFile.hpp"



class SimpleWidget : public morda::Widget, public morda::Updateable, public morda::CharInputFocusable{
	std::shared_ptr<morda::ResTexture> tex;
	std::shared_ptr<morda::ResFont> fnt;
	
public:	
	SimpleWidget(const stob::Node* desc) :
			morda::Widget(desc)
	{
//		TRACE(<< "loading texture" << std::endl)
		this->tex = morda::App::Inst().resMan.Load<morda::ResTexture>("tex_sample");
		this->fnt = morda::App::Inst().resMan.Load<morda::ResFont>("fnt_main");
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
	
	bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId) override{
		TRACE(<< "OnMouseButton(): isDown = " << isDown << ", pos = " << pos << ", button = " << unsigned(button) << ", pointerId = " << pointerId << std::endl)
		if(!isDown){
			return false;
		}
		
		if(this->IsUpdating()){
			this->StopUpdating();
		}else{
			this->StartUpdating(30);
		}
		this->Focus();
		this->FocusCharInput();
		return true;
	}
	
	bool OnKey(bool isDown, morda::EKey keyCode) override{
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
	
	void OnCharacterInput(const ting::Buffer<const std::uint32_t>& unicode) override{
		if(unicode.size() == 0){
			return;
		}
		
		TRACE(<< "SimpleWidget::OnCharacterInput(): unicode = " << unicode[0] << std::endl)
	}
	
	void Render(const morda::Matr4r& matrix)const override{
		{
			morda::Matr4r matr(matrix);
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
	
	void Render(const morda::Matr4r& matrix)const override{
		this->Widget::Render(matrix);
		
		morda::Matr4r matr(matrix);
		matr.Scale(this->Rect().d / 2);
		matr.Translate(1, 1);
		matr.Frustum(-2, 2, -1.5, 1.5, 2, 100);
		
		morda::Matr4r m(matr);
		m.Translate(0, 0, -4);
		
		m.Rotate(this->rot);

		this->tex->Tex().Bind();
		
		morda::SimpleTexturingShader &s = morda::App::Inst().Shaders().simpleTexturing;
		s.Bind();
		s.EnablePositionPointer();
		s.EnableTexCoordPointer();
//		s.SetColor(morda::Vec3f(0, 1, 0));
		s.SetMatrix(m);
		
		
		static morda::Vec3f cubePos[] = {
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
			
		};
		s.SetPositionPointer(cubePos);
		
		static morda::Vec2f cubeTex[] = {
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
		};
		s.SetTexCoordPointer(cubeTex);
		
		glEnable(GL_CULL_FACE);
		
		s.DrawArrays(GL_TRIANGLES, 36);
	}
};


class Application : public morda::App{
	static morda::App::WindowParams GetWindowParams()NOEXCEPT{
		morda::App::WindowParams wp;
		
		wp.dim.x = 800;
		wp.dim.y = 800;
		
		return wp;
	}
public:
	Application() :
			App(GetWindowParams())
	{
		this->resMan.MountResPack(*this->CreateResourceFileInterface());
//		this->ResMan().MountResPack(morda::ZipFile::New(ting::fs::FSFile::New("res.zip")));
		
		this->inflater().AddWidget<SimpleWidget>("U_SimpleWidget");
		this->inflater().AddWidget<CubeWidget>("CubeWidget");

		std::shared_ptr<morda::Widget> c = morda::App::Inst().inflater().Inflate(
				*this->CreateResourceFileInterface("test.gui.stob")
			);

//		morda::ZipFile zf(ting::fs::FSFile::New("res.zip"), "test.gui.stob");
//		std::shared_ptr<morda::Widget> c = morda::App::Inst().inflater().Inflate(zf);
		
		
		std::dynamic_pointer_cast<morda::Button>(c->FindChildByName("show_VK_button"))->onPressed = [this](){
			this->ShowVirtualKeyboard();
		};
		
		std::dynamic_pointer_cast<morda::Button>(c->FindChildByName("Hello world button"))->onPressed = [this](){
			this->PostToUIThread_ts(
					[](){
						TRACE_ALWAYS(<< "Print from UI thread!!!!!!!!" << std::endl)
					}
				);
		};
		
		std::dynamic_pointer_cast<CubeWidget>(c->FindChildByName("cube_widget"))->StartUpdating(30);
		
		this->SetRootWidget(c);
	}
};



std::unique_ptr<morda::App> morda::CreateApp(int argc, const char** argv, ting::Buffer<const std::uint8_t> savedState){
	return std::unique_ptr<Application>(new Application());
}
