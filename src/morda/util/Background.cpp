#include "Background.hpp"

#include "../App.hpp"


using namespace morda;



Background::Background(const stob::Node& desc){
	if(desc == "color"){
		if(const stob::Node* p = desc.Child()){
			this->color = p->AsU32();
		}
	}else{
		throw morda::Exc("Background(stob): unknown background description");
	}
}



void Background::Render(const morda::Matr4f& matrix, const morda::Vec2f& dim) const{
	morda::Matr4f matr(matrix);
	matr.Scale(dim);
	
	SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	s.SetColor(this->color);
	
	s.SetMatrix(matr);
	s.DrawQuad01();
}
