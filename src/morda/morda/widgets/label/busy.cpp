#include "busy.hpp"

#include <utki/math.hpp>

using namespace morda;

busy::busy(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		image(this->context, desc)
{
	if(!this->get_image()){
		this->set_image(this->context->loader.load<res::image>("morda_img_busy"));
	}

	this->set_visible(false);

	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "active"){
			if(get_property_value(p).to_bool()){
				this->context->run_from_ui_thread([this](){
					this->set_active(true);
				});
			}
		}
	}
}

void busy::render(const matrix4& matrix)const{
	matrix4 matr(matrix);

	matr.translate(this->rect().d / 2);
	matr.rotate(this->angle);
	matr.translate(-this->rect().d / 2);

	this->image::render(matr);
}

void busy::set_active(bool active){
	this->set_visible(active);
	if(active){
		this->context->updater->start(utki::make_shared_from(*this));
	}else{
		this->context->updater->stop(*this);
	}
}

void busy::update(uint32_t dt_ms){
	angle += utki::deg_to_rad(real(180)) / real(1000) * real(dt_ms);
}
