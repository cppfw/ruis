#include "spinner.hpp"

#include <utki/math.hpp>

using namespace morda;

spinner::spinner(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		image(this->context, desc)
{}

void spinner::render(const matrix4& matrix)const{
	matrix4 matr(matrix);

	matr.translate(this->rect().d / 2);
	matr.rotate(this->angle);
	matr.translate(-this->rect().d / 2);

	this->image::render(matr);
}

void spinner::set_active(bool active){
	if(active){
		this->context->updater->start(utki::make_shared_from(*this));
	}else{
		this->context->updater->stop(*this);
	}
}

void spinner::update(uint32_t dt_ms){
	angle += utki::deg_to_rad(real(180)) / real(1000) * real(dt_ms);
}
