#include "TexturingShader.hpp"


using namespace morda;



namespace{
morda::Vec2f quadTexCoords[] = {
	morda::Vec2f(0, 1), morda::Vec2f(1, 1), morda::Vec2f(1, 0), morda::Vec2f(0, 0)
};
}//~namespace



void TexturingShader::DrawQuad(){
	this->EnableTexCoordPointer();
	this->SetTexCoordPointer(quadTexCoords);
	this->Shader::DrawQuad();
}



void TexturingShader::DrawQuad01(){
	this->EnableTexCoordPointer();
	this->SetTexCoordPointer(quadTexCoords);
	this->Shader::DrawQuad01();
}
