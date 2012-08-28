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



class SimpleWidget : public morda::Widget{
	ting::Ref<morda::ResTexture> tex;
	ting::Ref<morda::ResFont> fnt;
	
	SimpleWidget(ting::Ptr<stob::Node> properties) :
			morda::Widget(properties)
	{
//		TRACE(<< "loading texture" << std::endl)
		this->tex = morda::App::Inst().ResMan().Load<morda::ResTexture>("tex_sample");
		this->fnt = morda::App::Inst().ResMan().Load<morda::ResFont>("fnt_main");
	}
public:	
	static inline ting::Ref<SimpleWidget> New(ting::Ptr<stob::Node> properties){
		return ting::Ref<SimpleWidget>(
				new SimpleWidget(properties)
			);
	}
	
	//override
	void Render(const tride::Matr4f& matrix)const{
		{
			tride::Matr4f matr(matrix);
			matr.Scale(this->Rect().d);

			this->tex->Tex().Bind();

			morda::SimpleTexturingShader &s = morda::App::Inst().Shaders().simpleTexturing;
			s.Bind();
			s.EnablePositionPointer();
//			s.SetColor(tride::Vec3f(1, 0, 0));
			s.SetMatrix(matr);
			s.DrawQuad01();
		}
		
//		this->fnt->Fnt().RenderTex(s , matrix);
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		morda::SimpleTexturingShader &s = morda::App::Inst().Shaders().simpleTexturing;
		tride::Matr4f m(matrix);
		m.Translate(200, 200);
		this->fnt->Fnt().RenderString(s, m, "Hello World!");
	}
};



class SimpleWidgetFactory : public morda::GuiInflater::WidgetFactory{
public:
	//override
	ting::Ref<morda::Widget> Create(ting::Ptr<stob::Node> node)const{
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

		this->SetRootContainer(
				morda::App::Inst().Inflater().Inflate(
						*this->CreateResourceFileInterface("test.gui.stob")
					)
			);
	}
};



ting::Ptr<morda::App> morda::CreateApp(int argc, char** argv, const ting::Buffer<const ting::u8>& savedState){
	return ting::Ptr<Application>(new Application());
}
