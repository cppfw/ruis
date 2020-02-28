#include "scroll_area.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

scroll_area::scroll_area(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		container(nullptr, desc)
{}

bool scroll_area::on_mouse_button(bool isDown, const morda::vector2& pos, mouse_button button, unsigned pointerID){
	vector2 d = -this->curScrollPos;
	return this->container::on_mouse_button(isDown, pos - d, button, pointerID);
}

bool scroll_area::on_mouse_move(const morda::vector2& pos, unsigned pointerID){
	vector2 d = -this->curScrollPos;
	return this->container::on_mouse_move(pos - d, pointerID);
}

void scroll_area::render(const morda::matrix4& matrix)const{
	vector2 d = -this->curScrollPos;

	matrix4 matr(matrix);
	matr.translate(d);

	this->container::render(matr);
}

void scroll_area::clampScrollPos(){
	if(this->effectiveDim.x < 0){
		this->curScrollPos.x = 0;
	}

	if(this->effectiveDim.y < 0){
		this->curScrollPos.y = 0;
	}
}

void scroll_area::set_scroll_pos(const vector2& newScrollPos){
	this->curScrollPos = newScrollPos.rounded();

	this->clampScrollPos();
	this->updateScrollFactor();
}

void scroll_area::set_scroll_factor(const vector2& factor){
	vector2 newScrollPos = this->effectiveDim.comp_multiplied(factor);

	this->set_scroll_pos(newScrollPos);
}

void scroll_area::updateScrollFactor(){
	// at this point effective dimension should be updated
	vector2 factor = this->curScrollPos.comp_divided(this->effectiveDim);

	if(this->curScrollFactor == factor){
		return;
	}

	for(unsigned i = 0; i != 2; ++i){
		if(this->effectiveDim[i] <= 0){
			this->curScrollFactor[i] = 0;
		}else{
			this->curScrollFactor[i] = this->curScrollPos[i] / this->effectiveDim[i];
		}
	}
}

vector2 scroll_area::dims_for_widget(const widget& w, const layout_params& lp)const{
	vector2 d;
	for(unsigned i = 0; i != 2; ++i){
		if(lp.dims[i] == layout_params::fill){
			d[i] = this->rect().d[i];
		}else if(lp.dims[i] == layout_params::min || lp.dims[i] == layout_params::max){
			d[i] = -1; // will be updated below
		}else{
			d[i] = lp.dims[i];
		}
	}
	if(d.x < 0 || d.y < 0){
		vector2 md = w.measure(d);
		for(unsigned i = 0; i != md.size(); ++i){
			if(d[i] < 0){
				if(lp.dims[i] == layout_params::max && md[i] < this->rect().d[i]){
					d[i] = this->rect().d[i];
				}else{
					d[i] = md[i];
				}
			}
		}
	}
	return d;
}

void scroll_area::arrangeWidgets(){
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		// NOTE: const_cast() to force calling of constant version of get_layout_params() because
		//       non-constant version of it invalidates layout.
		auto& lp = const_cast<
				std::add_pointer<std::add_const<std::remove_pointer<decltype(this)>::type>::type>::type
			>(this)->get_layout_params(**i);

		auto d = this->dims_for_widget(**i, lp);

		(*i)->resize(d);
	}
}

void scroll_area::lay_out(){
	this->arrangeWidgets();
	this->updateEffectiveDim();

	// distance of content's bottom right corner from bottom right corner of the scroll_area
	vector2 br = this->curScrollPos - this->effectiveDim;

	if(br.x > 0){
		if(br.x <= this->curScrollPos.x){
			this->curScrollPos.x -= br.x;
		}else{
			this->curScrollPos.x = 0;
		}
	}

	if(br.y > 0){
		if(br.y <= this->curScrollPos.y){
			this->curScrollPos.y -= br.y;
		}else{
			this->curScrollPos.y = 0;
		}
	}
}

void scroll_area::on_children_changed(){
	this->container::on_children_changed();
	this->arrangeWidgets();
	this->updateEffectiveDim();
}

void scroll_area::updateEffectiveDim(){
	morda::vector2 minDim(0);

	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->get_layout_params_as<layout_params>(**i);

		morda::vector2 d = this->dims_for_widget(**i, lp) + (*i)->rect().p;

		utki::clampBottom(minDim.x, d.x);
		utki::clampBottom(minDim.y, d.y);
	}

	this->effectiveDim = minDim - this->rect().d;
	this->updateScrollFactor();
}

vector2 scroll_area::get_visible_area_fraction()const noexcept{
	auto ret = this->rect().d.comp_divided(this->rect().d + this->effectiveDim);

	for(unsigned i = 0; i != ret.size(); ++i){
		utki::clampTop(ret[i], real(1));
	}

	return ret;
}
