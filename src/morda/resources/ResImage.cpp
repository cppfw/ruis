#include <utki/Singleton.hpp>

#include "ResImage.hpp"

#include "../App.hpp"
#include "../util/util.hpp"
#include "../shaders/PosTexShader.hpp"


using namespace morda;



ResImage::ResImage(std::shared_ptr<ResTexture> tex, const Rectr& rect) :
		tex(tex),
		dim_var(rect.d.abs())
{
	this->texCoords[3] = Vec2r(rect.left(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
	this->texCoords[2] = Vec2r(rect.right(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
	this->texCoords[1] = Vec2r(rect.right(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
	this->texCoords[0] = Vec2r(rect.left(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
}



std::shared_ptr<ResImage> ResImage::load(const stob::Node& chain, const papki::File& fi){
	auto tex = App::inst().resMan.load<ResTexture>(chain.side("tex").up().value());
	
	Rectr rect;
	if(auto n = chain.childOfThisOrNext("rect")){
		rect = makeRectrFromSTOB(n);
	}else{
		rect = Rectr(Vec2r(0, 0), tex->tex().dim());
	}
	
	return utki::makeShared<ResImage>(tex, rect);
}



void ResImage::render(const Matr4r& matrix, PosTexShader& s) const{
	this->tex->tex().bind();
	
	kolme::Matr4f matr(matrix);
	matr.scale(this->dim_var);
	
	s.SetMatrix(matr);
	s.render(utki::wrapBuf(PosShader::quad01Fan), utki::wrapBuf(this->texCoords));
}

void ResImage::render(const Matr4r& matrix, Vec2r renderDim, PosTexShader& s) const {
	//TODO:
	this->render(matrix, s);
}
