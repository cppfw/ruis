#include "TextField.hpp"

#include "containers/BlackHoleContainer.hpp"

#include "../App.hpp"


using namespace morda;



TextField::TextField(const stob::Node* chain) :
		Widget(chain),
		ti(utki::makeShared<TextInput>(chain))
{
	this->SetNinePatch(morda::App::inst().resMan.Load<ResNinePatch>("morda_npt_textfield"));
	
	
	auto bhc = utki::makeShared<BlackHoleContainer>();
	bhc->setShrink(Vec2b(true, false));
	bhc->Add(this->ti);
	
	{
		auto& lp = bhc->getLayoutParams(*this->ti);
		lp.dim.x = Widget::LayoutParams::D_Max;
		lp.dim.y = Widget::LayoutParams::D_Min;
	}
	
	this->Content().Add(bhc);
	
	{
		auto& lp = this->Content().getLayoutParams(*bhc);

		lp.dim.x = Widget::LayoutParams::D_Max;
		lp.dim.y = Widget::LayoutParams::D_Min;
	}
}
