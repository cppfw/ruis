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

		if(this->parent() && this->parent()->children().front().get() != this){ //if this is not the first widget in the parent
			auto prevIter = this->parent()->find(this);
			ASSERT(prevIter != this->parent()->children().end())
			--prevIter;

			if(auto pt = dynamic_cast<Tab*>(prevIter->get())){ //previous tab
				if(pt->isPressed()){
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


bool Tab::on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId) {
	if(isDown){
		if(!maskOverlaps(pos)){
			return false;
		}
	}

	return this->ChoiceButton::on_mouse_button(isDown, pos, button, pointerId);
}

void Tab::onPressedChanged() {
	this->ChoiceButton::onPressedChanged();
	this->NinePatchToggle::onPressedChanged();
}


Tab::Tab(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		Button(c, desc),
		ToggleButton(c, desc),
		ChoiceButton(c, desc),
		NinePatchToggle(c, desc)
{
	if(!this->pressedNinePatch()){
		this->setPressedNinePatch(morda::inst().context->loader.load<ResNinePatch>("morda_npt_tab_active"));
	}
	if(!this->unpressedNinePatch()){
		this->setUnpressedNinePatch(morda::inst().context->loader.load<ResNinePatch>("morda_npt_tab_inactive"));
	}

	// initialize nine-patch
	this->onPressedChanged();
}
