#include "../../src/morda/App.hpp"
#include "../../src/morda/Widget.hpp"

#include "../../src/morda/shaders/SimpleSingleColoringShader.hpp"
#include "../../src/morda/shaders/SimpleTexturingShader.hpp"

#include "../../src/morda/resources/ResTexture.hpp"
#include "../../src/morda/resources/ResFont.hpp"


#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>



class SimpleWidget : public morda::Widget{
	ting::Ref<morda::ResTexture> tex;
	ting::Ref<morda::ResFont> fnt;
	
	SimpleWidget(){
//		TRACE(<< "loading texture" << std::endl)
		this->tex = morda::App::Inst().ResMan().Load<morda::ResTexture>("tex_sample");
		this->fnt = morda::App::Inst().ResMan().Load<morda::ResFont>("fnt_main");
	}
public:	
	static inline ting::Ref<SimpleWidget> New(){
		return ting::Ref<SimpleWidget>(
				new SimpleWidget()
			);
	}
	
	//override
	void Render(const tride::Matr4f& matrix)const{
		tride::Matr4f matr(matrix);
		matr.Scale(this->Rect().d);
		
		this->tex->Tex().Bind();
		
		morda::SimpleTexturingShader &s = morda::SimpleTexturingShader::Inst();
		s.Bind();
		s.EnablePositionPointer();
//		s.SetColor(tride::Vec3f(1, 0, 0));
		s.SetMatrix(matr);
		s.DrawQuad01();
		
#ifdef DEBUG
		this->fnt->Fnt().RenderTex(s , matrix);
#endif
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		tride::Matr4f m(matrix);
		m.Translate(200, 200);
		this->fnt->Fnt().RenderString(s, m, "Hello World!");
	}
};



int main(int argc, char** argv){
	morda::App app(320, 480);
	
	app.ResMan().MountResPack(ting::Ptr<ting::fs::File>(new ting::fs::FSFile()));
	
	morda::SimpleSingleColoringShader simpleSingleColoringShader;
	morda::SimpleTexturingShader simpleTexturingShader;
	
	app.SetRootWidget(SimpleWidget::New());
	
	app.Exec();
	
	return 0;
}
