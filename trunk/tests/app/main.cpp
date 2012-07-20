#include "../../src/morda/App.hpp"
#include "../../src/morda/Widget.hpp"

#include "../../src/morda/shaders/SimpleSingleColoringShader.hpp"
#include "../../src/morda/shaders/SimpleTexturingShader.hpp"

#include "../../src/morda/resources/ResTexture.hpp"


#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>



class SimpleWidget : public morda::Widget{
	ting::Ref<morda::ResTexture> tex;
	
	SimpleWidget(){
//		TRACE(<< "loading texture" << std::endl)
		this->tex = morda::App::Inst().ResMan().Load<morda::ResTexture>("tex_sample");
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
		matr.Scale(this->Dim());
		
		this->tex->operator ->()->Bind();
		
		morda::SimpleTexturingShader &s = morda::SimpleTexturingShader::Inst();
		s.Bind();
		s.EnablePositionPointer();
//		s.SetColor(tride::Vec3f(1, 0, 0));
		s.SetMatrix(matr);
		s.DrawQuad01();
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
