#include "tab.hpp"
#include "../../util/util.hpp"

using namespace morda;

bool tab::maskOverlaps(Vec2r pos) {
	auto borders = this->get_actual_borders();

	if(pos.x < borders.left()){
		if(pos.y != 0 && this->rect().d.y != 0){
			if(pos.x / (this->rect().d.y - pos.y) < borders.left() / this->rect().d.y){
				return false;
			}
		}

		if(this->parent() && this->parent()->children().front().get() != this){ // if this is not the first widget in the parent
			auto prevIter = this->parent()->find(this);
			ASSERT(prevIter != this->parent()->children().end())
			--prevIter;

			if(auto pt = dynamic_cast<tab*>(prevIter->get())){ // previous tab
				if(pt->is_pressed()){
					if(pt->maskOverlaps(pos + this->rect().p - pt->rect().p)){
						return false;
					}
				}
			}
		}
	}else{
		auto rb = this->rect().d.x - borders.right();
		auto dx = pos.x - rb;
		if(dx > 0){
			if(pos.y != 0 && this->rect().d.y != 0){
				if(dx / pos.y > borders.right() / this->rect().d.y){
					return false;
				}
			}
		}
	}
	return true;
}

bool tab::on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId){
	if(isDown){
		if(!maskOverlaps(pos)){
			return false;
		}
	}

	return this->choice_button::on_mouse_button(isDown, pos, button, pointerId);
}

void tab::on_pressed_changed() {
	this->choice_button::on_pressed_changed();
	this->nine_patch_toggle::on_pressed_changed();
}

tab::tab(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc),
		choice_button(this->context, desc),
		nine_patch_toggle(this->context, desc)
{
	if(!this->get_pressed_nine_patch()){
		this->set_pressed_nine_patch(this->context->loader.load<res_nine_patch>("morda_npt_tab_active"));
	}
	if(!this->get_unpressed_nine_patch()){
		this->set_unpressed_nine_patch(this->context->loader.load<res_nine_patch>("morda_npt_tab_inactive"));
	}

	// initialize nine-patch
	this->on_pressed_changed();
}
