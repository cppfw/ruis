#include "TableRow.hpp"


using namespace morda;


TableRow::TableRow(const stob::Node* chain) :
		Widget(chain),
		HorizontalContainer(chain)
{ }


TableRow::LayoutParams::LayoutParams(const stob::Node* chain) :
		HorizontalContainer::LayoutParams(chain),
		modifiedParams(nullptr)
{
}



morda::LayoutParams& TableRow::GetLayoutParams(Widget& w){
	auto lp = dynamic_cast<LayoutParams*>(&this->HorizontalContainer::GetLayoutParams(w));
	
	if(!lp){
		throw morda::Exc("TableRow: layout parameters can only be of type TableRow::LayoutParams, other type found");
	}
	
	return lp->modifiedParams;
}
