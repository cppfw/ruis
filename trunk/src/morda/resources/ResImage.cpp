#include <ting/Singleton.hpp>

#include "ResImage.hpp"

#include "../App.hpp"
#include "../util/util.hpp"
#include "../shaders/TexturingShader.hpp"


using namespace morda;



ResImage::ResImage(std::shared_ptr<ResTexture> tex, const Rect2r& rect) :
		tex(tex),
		dim(rect.d)
{
	this->texCoords[0] = Vec2r(rect.Left(), rect.Bottom()).CompDivBy(this->tex->Tex().Dim());
	this->texCoords[1] = Vec2r(rect.Right(), rect.Bottom()).CompDivBy(this->tex->Tex().Dim());
	this->texCoords[2] = Vec2r(rect.Right(), rect.Top()).CompDivBy(this->tex->Tex().Dim());
	this->texCoords[3] = Vec2r(rect.Left(), rect.Top()).CompDivBy(this->tex->Tex().Dim());
}



std::shared_ptr<ResImage> ResImage::Load(const stob::Node& r, const ting::fs::File& fi){
	decltype(ResImage::tex) tex;
	if(const stob::Node* n = r.GetProperty("tex")){
		tex = App::Inst().resMan.Load<ResTexture>(n->Value());
	}else{
		throw ResourceManager::Exc("ResImage::Load(): no 'tex' property found");
	}
	
	Rect2r rect;
	if(const stob::Node* n = r.GetProperty("rect")){
		rect = Rect2rFromSTOB(n);
	}else{
		throw ResourceManager::Exc("ResImage::Load(): no 'rect' property found");
	}
	
	return ting::New<ResImage>(tex, rect);
}



void ResImage::Render(const Matr4r& matrix, TexturingShader& s) const{
	this->tex->Tex().Bind();
	
	Matr4f matr(matrix);
	matr.Scale(this->dim);
	
	s.SetMatrix(matr);
	s.EnableTexCoordPointer();
	s.SetTexCoordPointer(&*this->texCoords.begin());
	s.Shader::DrawQuad01();
}
