#include "../../src/morda/Application.hpp"
#include "../../src/morda/Widget.hpp"

#include "../../src/morda/shaders/SimpleSingleColoringShader.hpp"


#include <ting/debug.hpp>



class SimpleWidget : public morda::Widget{
	SimpleWidget(){}
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
		
		morda::SimpleSingleColoringShader &s = morda::SimpleSingleColoringShader::Inst();
		s.Bind();
		s.EnablePositionPointer();
		s.SetColor(tride::Vec3f(1, 0, 0));
		s.SetMatrix(matr);
		s.DrawQuad();
	}
};



int main(int argc, char** argv){
	morda::Application app(320, 480);
	
	morda::SimpleSingleColoringShader simpleSingleColoringShader;
	
	app.SetRootWidget(SimpleWidget::New());
	
	app.Exec();
	
	return 0;
}
