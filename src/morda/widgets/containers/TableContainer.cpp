#include <vector>

#include "TableContainer.hpp"



using namespace morda;



TableContainer::TableContainer(const stob::Node* chain) :
		Widget(chain),
		VerticalContainer(chain)
{}



void TableContainer::OnResize(){
	std::vector<std::tuple<TableRow*, morda::Widget::T_ChildrenList::const_iterator>> iterators;
	iterators.reserve(this->Children().size());
	
	for(auto& c : this->Children()){
		auto tr = dynamic_cast<TableRow*>(c.get());
		if(!tr){
			throw morda::Exc("TableContainer: non-TableRow child found, TableContainer can only hold TableRow children");
		}
		
		iterators.push_back(std::make_tuple(tr, tr->Children().begin()));
	}
	
//	real maxDimX = 0;
//	real maxWeight = 0;
//	
//	for(auto& i : iterators){
//		auto tr = std::get<0>(i);
//		auto& iter = std::get<1>(i);
//		
//		if(iter != tr->Children().end()){
//			const auto layoutParams = &tr->GetLayoutParams(**iter);
//			const auto lp = dynamic_cast<const TableRow::LayoutParams*>(layoutParams);
////			auto& lp = tr->GetLayoutParamsAs<TableRow::LayoutParams>(**iter);
//			ting::util::ClampBottom(maxDimX, lp->dim.x);
//			ting::util::ClampBottom(maxWeight, lp->weight);
//		}
//	}
	//TODO:
	
	this->VerticalContainer::OnResize();
}

morda::LayoutParams& TableContainer::GetLayoutParams(Widget& w){
	auto& layoutParams = dynamic_cast<LayoutParams&>(this->VerticalContainer::GetLayoutParams(w));
	
	layoutParams.fill.x = true;
	
	return layoutParams;
}
