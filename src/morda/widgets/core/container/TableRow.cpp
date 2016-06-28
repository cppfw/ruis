#include "TableRow.hpp"


using namespace morda;


TableRow::TableRow(const stob::Node* chain) :
		Widget(chain),
		HorizontalArea(chain)
{}


TableRow::LayoutParams::LayoutParams(const stob::Node* chain) :
		HorizontalArea::LayoutParams(chain),
		processedParams(nullptr)
{}



const Widget::LayoutParams& TableRow::getLayoutParamsDuringLayout(const Widget& w)const{
	auto lp = dynamic_cast<const LayoutParams*>(&this->getLayoutParams(w));
	
	if(!lp){
		throw morda::Exc("TableRow: layout parameters can only be of type TableRow::LayoutParams, other type found");
	}
	
	return lp->processedParams;
}



const TableRow::LayoutParams& TableRow::getTableRowLayoutParams(const Widget& w)const{
	auto lp = dynamic_cast<const LayoutParams*>(&this->getLayoutParams(w));
	if(!lp){
		throw morda::Exc("TableRow: layout parameters can only be of type TableRow::LayoutParams, other type found");
	}
	return *lp;
}
