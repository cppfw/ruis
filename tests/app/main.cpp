#include "../../src/morda/App.hpp"
#include "../../src/morda/Widget.hpp"
#include "../../src/morda/Container.hpp"

#include "../../src/morda/widgets/Button.hpp"
#include "../../src/morda/widgets/Label.hpp"

#include "../../src/morda/shaders/SimpleSingleColoringShader.hpp"
#include "../../src/morda/shaders/SimpleTexturingShader.hpp"

#include "../../src/morda/resources/ResTexture.hpp"
#include "../../src/morda/resources/ResFont.hpp"


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

			morda::SimpleTexturingShader &s = morda::App::Inst().shaders.simpleTexturing;
			s.Bind();
			s.EnablePositionPointer();
//			s.SetColor(tride::Vec3f(1, 0, 0));
			s.SetMatrix(matr);
			s.DrawQuad01();
		}
		
//		this->fnt->Fnt().RenderTex(s , matrix);
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		morda::SimpleTexturingShader &s = morda::App::Inst().shaders.simpleTexturing;
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



int main(int argc, char** argv){
	morda::App app(320, 480);
	
	app.ResMan().MountResPack(ting::Ptr<ting::fs::File>(new ting::fs::FSFile()));
		
	app.Inflater().AddWidgetFactory("U_SimpleWidget", ting::Ptr<morda::GuiInflater::WidgetFactory>(new SimpleWidgetFactory()));
	
	ting::fs::FSFile fi("test.gui.stob");
	app.SetRootWidget(morda::App::Inst().Inflater().Inflate(fi));
	
	app.Exec();
	
	return 0;
}
