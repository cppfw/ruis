#include "TexturingShader.hpp"


using namespace morda;



namespace{
tride::Vec2f quadTexCoords[] = {
	tride::Vec2f(0, 0), tride::Vec2f(0, 1), tride::Vec2f(1, 1), tride::Vec2f(1, 0)
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
