#include "Tab.hpp"
#include "../../util/util.hpp"

using namespace morda;

bool Tab::maskOverlaps(Vec2r pos) {
	auto borders = this->getActualBorders();

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

			if(auto pt = dynamic_cast<Tab*>(prevIter->get())){ // previous tab
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

bool Tab::on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId){
	if(isDown){
		if(!maskOverlaps(pos)){
			return false;
		}
	}

	return this->choice_button::on_mouse_button(isDown, pos, button, pointerId);
}

void Tab::on_pressed_changed() {
	this->choice_button::on_pressed_changed();
	this->NinePatchToggle::on_pressed_changed();
}

Tab::Tab(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		ToggleButton(this->context, desc),
		choice_button(this->context, desc),
		NinePatchToggle(this->context, desc)
{
	if(!this->pressedNinePatch()){
		this->setPressedNinePatch(this->context->loader.load<res_ninepatch>("morda_npt_tab_active"));
	}
	if(!this->unpressedNinePatch()){
		this->setUnpressedNinePatch(this->context->loader.load<res_ninepatch>("morda_npt_tab_inactive"));
	}

	// initialize nine-patch
	this->on_pressed_changed();
}
