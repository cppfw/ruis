#include <ting/Singleton.hpp>

#include "ResImage.hpp"

#include "../App.hpp"
#include "../util/util.hpp"
#include "../shaders/PosTexShader.hpp"


using namespace morda;



ResImage::ResImage(std::shared_ptr<ResTexture> tex, const Rect2r& rect) :
		tex(tex),
		dim(rect.d.Abs())
{
	this->texCoords[3] = Vec2r(rect.Left(), this->tex->Tex().Dim().y - rect.Bottom()).CompDivBy(this->tex->Tex().Dim());
	this->texCoords[2] = Vec2r(rect.Right(), this->tex->Tex().Dim().y - rect.Bottom()).CompDivBy(this->tex->Tex().Dim());
	this->texCoords[1] = Vec2r(rect.Right(), this->tex->Tex().Dim().y - rect.Top()).CompDivBy(this->tex->Tex().Dim());
	this->texCoords[0] = Vec2r(rect.Left(), this->tex->Tex().Dim().y - rect.Top()).CompDivBy(this->tex->Tex().Dim());
}



std::shared_ptr<ResImage> ResImage::Load(const stob::Node& chain, const ting::fs::File& fi){
	auto tex = App::Inst().resMan.Load<ResTexture>(chain.getThisOrNext("tex").getChild().Value());
	
	Rect2r rect;
	if(auto n = chain.ChildOfThisOrNext("rect")){
		rect = Rect2rFromSTOB(n);
	}else{
		rect = Rect2r(Vec2r(0, 0), tex->Tex().Dim());
	}
	
	return ting::New<ResImage>(tex, rect);
}



void ResImage::Render(const Matr4r& matrix, PosTexShader& s) const{
	this->tex->Tex().Bind();
	
	Matr4f matr(matrix);
	matr.Scale(this->dim);
	
	s.Bind();
	s.SetMatrix(matr);
	s.Render(PosShader::quad01Fan, this->texCoords, Shader::EMode::TRIANGLE_FAN);
}
